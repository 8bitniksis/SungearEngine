//
// Created by stuka on 07.07.2023.
//

#include "VkTexture2D.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

#include <glm/gtc/packing.hpp>

#include "RHI/VulkanDevice.h"
#include "RHI/VulkanGPUBuffer.h"
#include "RHI/VulkanTextureUnits.h"
#include "SGCore/Graphics/API/IFrameBuffer.h"
#include "SGCore/Logger/Logger.h"
#include "VkRenderer.h"
#include "VulkanTypesCaster.h"

namespace
{
    SGCore::VulkanDevice* currentDevice() noexcept
    {
        // never through getInstance(): this runs from asset destructors, which happen in static
        // destruction after the renderer singleton is gone
        return SGCore::VkRenderer::getLiveDevice();
    }

    std::uint32_t bytesPerChannel(SGGDataType type) noexcept
    {
        const auto size = getSGGDataTypeSizeInBytes(type);
        return size == 0 ? 1 : size;
    }

    using FormatChannelKind = SGCore::VulkanTypesCaster::FormatChannelKind;

    /// One channel of the CPU buffer in the value space GL uploads through: byte channels come out
    /// normalized, float and integer channels keep their magnitude.
    float decodeSourceChannel(const std::uint8_t* source, SGGDataType type) noexcept
    {
        switch(type)
        {
            case SGGDataType::SGG_FLOAT: { float value; std::memcpy(&value, source, sizeof(value)); return value; }
            case SGGDataType::SGG_BYTE: return std::max(static_cast<float>(*reinterpret_cast<const std::int8_t*>(source)) / 127.0f, -1.0f);
            case SGGDataType::SGG_UNSIGNED_SHORT: { std::uint16_t value; std::memcpy(&value, source, sizeof(value)); return static_cast<float>(value); }
            case SGGDataType::SGG_SHORT: { std::int16_t value; std::memcpy(&value, source, sizeof(value)); return static_cast<float>(value); }
            case SGGDataType::SGG_UNSIGNED_INT: { std::uint32_t value; std::memcpy(&value, source, sizeof(value)); return static_cast<float>(value); }
            case SGGDataType::SGG_INT: { std::int32_t value; std::memcpy(&value, source, sizeof(value)); return static_cast<float>(value); }
            default: return static_cast<float>(*source) / 255.0f; // SGG_UNSIGNED_BYTE and anything byte-shaped
        }
    }

    /// Writes a decoded channel in the image's own layout.
    void encodeImageChannel(std::uint8_t* destination, FormatChannelKind kind, std::uint32_t size, float value) noexcept
    {
        switch(kind)
        {
            case FormatChannelKind::UNORM:
            {
                const float clamped = std::clamp(value, 0.0f, 1.0f);
                if(size == 1) { *destination = static_cast<std::uint8_t>(std::lround(clamped * 255.0f)); return; }
                const auto encoded = static_cast<std::uint16_t>(std::lround(clamped * 65535.0f));
                std::memcpy(destination, &encoded, sizeof(encoded));
                return;
            }
            case FormatChannelKind::SNORM:
            {
                const float clamped = std::clamp(value, -1.0f, 1.0f);
                if(size == 1) { const auto e = static_cast<std::int8_t>(std::lround(clamped * 127.0f)); std::memcpy(destination, &e, sizeof(e)); return; }
                const auto encoded = static_cast<std::int16_t>(std::lround(clamped * 32767.0f));
                std::memcpy(destination, &encoded, sizeof(encoded));
                return;
            }
            case FormatChannelKind::UINT:
            {
                const auto magnitude = static_cast<std::uint32_t>(std::llround(std::max(value, 0.0f)));
                if(size == 1) { *destination = static_cast<std::uint8_t>(magnitude); return; }
                if(size == 2) { const auto e = static_cast<std::uint16_t>(magnitude); std::memcpy(destination, &e, sizeof(e)); return; }
                std::memcpy(destination, &magnitude, sizeof(magnitude));
                return;
            }
            case FormatChannelKind::SINT:
            {
                const auto magnitude = static_cast<std::int32_t>(std::llround(value));
                if(size == 1) { const auto e = static_cast<std::int8_t>(magnitude); std::memcpy(destination, &e, sizeof(e)); return; }
                if(size == 2) { const auto e = static_cast<std::int16_t>(magnitude); std::memcpy(destination, &e, sizeof(e)); return; }
                std::memcpy(destination, &magnitude, sizeof(magnitude));
                return;
            }
            case FormatChannelKind::SFLOAT:
            {
                if(size == 2) { const std::uint16_t half = glm::packHalf1x16(value); std::memcpy(destination, &half, sizeof(half)); return; }
                std::memcpy(destination, &value, sizeof(value));
                return;
            }
            default:
                return;
        }
    }

    /// True when the CPU data type is stored exactly as the image stores a channel.
    bool sourceMatchesImage(SGGDataType type, const SGCore::VulkanTypesCaster::FormatLayout& layout) noexcept
    {
        switch(type)
        {
            case SGGDataType::SGG_FLOAT: return layout.m_kind == FormatChannelKind::SFLOAT && layout.m_channelSize == 4;
            case SGGDataType::SGG_UNSIGNED_BYTE: return layout.m_kind == FormatChannelKind::UNORM && layout.m_channelSize == 1;
            case SGGDataType::SGG_BYTE: return layout.m_kind == FormatChannelKind::SNORM && layout.m_channelSize == 1;
            case SGGDataType::SGG_UNSIGNED_SHORT: return layout.m_kind == FormatChannelKind::UINT && layout.m_channelSize == 2;
            case SGGDataType::SGG_SHORT: return layout.m_kind == FormatChannelKind::SINT && layout.m_channelSize == 2;
            case SGGDataType::SGG_UNSIGNED_INT: return layout.m_kind == FormatChannelKind::UINT && layout.m_channelSize == 4;
            case SGGDataType::SGG_INT: return layout.m_kind == FormatChannelKind::SINT && layout.m_channelSize == 4;
            default: return false;
        }
    }
}

SGCore::VkTexture2D::~VkTexture2D() noexcept
{
    destroyOnGPU();
}

void SGCore::VkTexture2D::create()
{
    auto* device = currentDevice();
    if(!device || !device->isReady()) return;
    if(m_width <= 0 || m_height <= 0) return;

    destroyOnGPU();

    VulkanTextureDesc desc;
    desc.m_width = static_cast<std::uint32_t>(m_width);
    desc.m_height = static_cast<std::uint32_t>(m_height);
    desc.m_format = VulkanTypesCaster::sggInternalFormatToVk(m_internalFormat);
    desc.m_usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    desc.m_mipLevels = 1;
    desc.m_debugName = "texture2d";
    m_vulkanTexture = VulkanTexture::create(device->getContext(), desc);
    if(!m_vulkanTexture) return;

    if(m_textureData)
    {
        uploadRegion(m_textureData.get(), desc.m_width, desc.m_height, 0, 0);
    }
    else
    {
        // no pixels: still leave the image in the layout shaders expect
        device->immediateSubmit([&](VkCommandBuffer commandBuffer) {
            m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });
    }
}

void SGCore::VkTexture2D::createAsFrameBufferAttachment(IFrameBuffer* parentFrameBuffer, SGFrameBufferAttachmentType attachmentType)
{
    auto* device = currentDevice();
    if(!device || !device->isReady() || !parentFrameBuffer) return;

    destroyOnGPU();
    m_frameBufferAttachmentType = attachmentType;

    const bool depth = isDepthAttachment(attachmentType) || isDepthStencilAttachment(attachmentType) ||
                       VulkanTypesCaster::isDepthFormat(m_internalFormat);

    VulkanTextureDesc desc;
    desc.m_width = static_cast<std::uint32_t>(std::max(1, m_width));
    desc.m_height = static_cast<std::uint32_t>(std::max(1, m_height));
    desc.m_format = VulkanTypesCaster::sggInternalFormatToVk(m_internalFormat);
    desc.m_usage = (depth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) |
                   VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    desc.m_samples = m_useMultisampling ? static_cast<VkSampleCountFlagBits>(m_multisamplingSamplesCount) : VK_SAMPLE_COUNT_1_BIT;
    // NEAREST, as GL4Texture2D creates its attachments: later passes sample attachments texel by
    // texel and compare the result exactly (the layered FX gate gets its layer volume that way), and
    // an interpolated sample never equals the stored texel
    desc.m_filter = VK_FILTER_NEAREST;
    desc.m_addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    // the framebuffer address is part of the name: the engine has several framebuffers with the same
    // attachment slots, and identical names make a RenderDoc capture unreadable
    desc.m_debugName = "attachment_" + sgFrameBufferAttachmentTypeToString(attachmentType) + "@" +
                       std::to_string(reinterpret_cast<std::uintptr_t>(parentFrameBuffer) & 0xFFFFu) + "_" +
                       std::to_string(desc.m_width) + "x" + std::to_string(desc.m_height);
    m_vulkanTexture = VulkanTexture::create(device->getContext(), desc);
    if(!m_vulkanTexture) return;

    device->immediateSubmit([&](VkCommandBuffer commandBuffer) {
        // A fresh Vulkan image holds whatever was in that memory, while a GL texture created without
        // data reads as zeros — and the engine relies on that: render passes load their attachments
        // (LoadOp::SGG_LOAD) and the FX framebuffer's attachments are never cleared at all, so
        // undefined contents survived into the final composite as sparse bright noise. Start cleared.
        m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkImageSubresourceRange range { };
        range.aspectMask = m_vulkanTexture->getAspect();
        range.levelCount = VK_REMAINING_MIP_LEVELS;
        range.layerCount = VK_REMAINING_ARRAY_LAYERS;

        if(m_vulkanTexture->isDepth())
        {
            // the far plane, not zero: a pass that forgets to clear depth should reject nothing
            const VkClearDepthStencilValue value { 1.0f, 0 };
            vkCmdClearDepthStencilImage(commandBuffer, m_vulkanTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &range);
        }
        else
        {
            const VkClearColorValue value { };
            vkCmdClearColorImage(commandBuffer, m_vulkanTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &range);
        }

        // rest in SHADER_READ_ONLY so an attachment can be sampled even before its first render pass
        m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    });
}

void SGCore::VkTexture2D::uploadRegion(const std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t x, std::uint32_t y) noexcept
{
    auto* device = currentDevice();
    if(!device || !m_vulkanTexture || !data || width == 0 || height == 0) return;

    const std::uint32_t channelSize = bytesPerChannel(m_dataType);
    const std::uint32_t srcChannels = static_cast<std::uint32_t>(std::max(1, m_channelsCount));
    const bool expand = VulkanTypesCaster::needsAlphaExpansion(m_internalFormat) && srcChannels == 3;
    const std::uint32_t dstChannels = expand ? 4 : srcChannels;
    const std::uint64_t srcPixel = static_cast<std::uint64_t>(srcChannels) * channelSize;
    const std::uint64_t dstPixel = static_cast<std::uint64_t>(dstChannels) * channelSize;
    const std::uint64_t pixelCount = static_cast<std::uint64_t>(width) * height;

    // The copy is sized by the image format, and glTexImage2D converts the CPU buffer into the internal
    // format on the way in while vkCmdCopyBufferToImage copies bytes as they are. The engine's
    // (channels, dataType) pair does not always agree with the declared internal format — the SSAO
    // noise hands 32-bit floats to an RGB16F image — so the staging buffer is built in the image's
    // layout, converting channel by channel. This is the upload counterpart of the decoding
    // VkFrameBuffer::readAttachmentPixels does.
    const auto imageLayout = VulkanTypesCaster::formatLayout(m_vulkanTexture->getFormat());
    const std::uint32_t texelSize = VulkanTypesCaster::formatTexelSize(m_vulkanTexture->getFormat());
    if(texelSize == 0 || imageLayout.m_kind == FormatChannelKind::UNKNOWN || imageLayout.m_channelSize == 0)
    {
        SG_LOG_E("VkTexture2D: can not upload '{}': image format {} has no plain channel layout.",
                 m_vulkanTexture->getDebugName(), static_cast<int>(m_vulkanTexture->getFormat()));

        // an image left in UNDEFINED is still bound and sampled by the passes, which is a validation
        // error on top of the refused upload: give it defined contents and the layout shaders expect
        device->immediateSubmit([&](VkCommandBuffer commandBuffer) {
            m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkImageSubresourceRange range { };
            range.aspectMask = m_vulkanTexture->getAspect();
            range.levelCount = VK_REMAINING_MIP_LEVELS;
            range.layerCount = VK_REMAINING_ARRAY_LAYERS;
            const VkClearColorValue value { };
            vkCmdClearColorImage(commandBuffer, m_vulkanTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &range);

            m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        });
        return;
    }

    auto staging = device->createStagingBuffer(pixelCount * texelSize, "texture_upload");
    if(!staging) return;

    auto* dst = static_cast<std::uint8_t*>(staging->getMappedPointer());

    if(dstPixel == texelSize && sourceMatchesImage(m_dataType, imageLayout) && !imageLayout.m_reversedChannels)
    {
        // identical layouts: the fast path the 8-bit textures of the engine take
        if(!expand)
        {
            std::memcpy(dst, data, pixelCount * srcPixel);
        }
        else
        {
            // RGB -> RGBA with an opaque alpha of the channel's maximum value
            for(std::uint64_t i = 0; i < pixelCount; ++i)
            {
                std::memcpy(dst + i * dstPixel, data + i * srcPixel, srcPixel);
                std::memset(dst + i * dstPixel + srcPixel, 0xFF, channelSize);
            }
        }
    }
    else
    {
        const auto channelsToConvert = std::min<std::uint32_t>(srcChannels, imageLayout.m_channels);
        for(std::uint64_t i = 0; i < pixelCount; ++i)
        {
            for(std::uint32_t channel = 0; channel < imageLayout.m_channels; ++channel)
            {
                const std::uint32_t target = imageLayout.m_reversedChannels && channel < 3 ? 2 - channel : channel;
                std::uint8_t* out = dst + i * texelSize + static_cast<std::uint64_t>(target) * imageLayout.m_channelSize;

                // channels the source does not carry (the alpha of a widened RGB image) read as opaque
                const float value = channel < channelsToConvert
                                        ? decodeSourceChannel(data + i * srcPixel + static_cast<std::uint64_t>(channel) * channelSize, m_dataType)
                                        : 1.0f;
                encodeImageChannel(out, imageLayout.m_kind, imageLayout.m_channelSize, value);
            }
        }
    }

    device->immediateSubmit([&](VkCommandBuffer commandBuffer) {
        m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkBufferImageCopy region { };
        region.bufferOffset = 0;
        region.imageSubresource.aspectMask = m_vulkanTexture->getAspect();
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = { static_cast<std::int32_t>(x), static_cast<std::int32_t>(y), 0 };
        region.imageExtent = { width, height, 1 };
        vkCmdCopyBufferToImage(commandBuffer, staging->getHandle(), m_vulkanTexture->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        m_vulkanTexture->recordTransition(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    });
}

void SGCore::VkTexture2D::subTextureDataOnGAPISide(const std::uint8_t* data, std::size_t areaWidth, std::size_t areaHeight,
                                                   std::size_t areaOffsetX, std::size_t areaOffsetY) noexcept
{
    uploadRegion(data, static_cast<std::uint32_t>(areaWidth), static_cast<std::uint32_t>(areaHeight),
                 static_cast<std::uint32_t>(areaOffsetX), static_cast<std::uint32_t>(areaOffsetY));
}

void SGCore::VkTexture2D::destroyOnGPU() noexcept
{
    if(!m_vulkanTexture) return;
    if(auto* device = currentDevice())
    {
        // the GPU may still read it: released with the next retired submission
        device->destroyDeferred(m_vulkanTexture);
    }
    m_vulkanTexture.reset();
}

void SGCore::VkTexture2D::bind(const std::uint8_t& textureUnit) const noexcept
{
    // Vulkan has no texture units: record "unit N holds this texture" and let VkShader join it with
    // the sampler->unit half at draw time (see VulkanTextureUnits)
    VulkanTextureUnits::set(textureUnit, m_vulkanTexture);
}

void* SGCore::VkTexture2D::getTextureNativeHandler() const noexcept
{
    return m_vulkanTexture.get();
}

void* SGCore::VkTexture2D::getTextureBufferNativeHandler() const noexcept
{
    return nullptr;
}

SGCore::VkTexture2D& SGCore::VkTexture2D::operator=(const Ref<ITexture2D>& other)
{
    // same contract as GL4Texture2D: the GPU object is not shared, only the CPU-side description
    if(!other) return *this;
    m_type = other->m_type;
    m_internalFormat = other->m_internalFormat;
    m_format = other->m_format;
    m_dataType = other->m_dataType;
    m_channelsCount = other->m_channelsCount;
    return *this;
}
