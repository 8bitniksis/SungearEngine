//
// Created by stuka on 07.07.2023.
//

#include "VkTexture2D.h"

#include <cstring>
#include <vector>

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
    // attachments are sampled by later passes without filtering surprises
    desc.m_filter = VK_FILTER_LINEAR;
    desc.m_addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    desc.m_debugName = "attachment_" + sgFrameBufferAttachmentTypeToString(attachmentType);
    m_vulkanTexture = VulkanTexture::create(device->getContext(), desc);
    if(!m_vulkanTexture) return;

    // rest in SHADER_READ_ONLY so an attachment can be sampled even before its first render pass
    device->immediateSubmit([&](VkCommandBuffer commandBuffer) {
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

    // the copy is sized by the image format, so the staging layout must match it exactly; the
    // engine's (channels, dataType) pair does not always agree with the declared internal format
    const std::uint32_t texelSize = VulkanTypesCaster::formatTexelSize(m_vulkanTexture->getFormat());
    if(texelSize == 0 || dstPixel != texelSize)
    {
        SG_LOG_E("VkTexture2D: can not upload '{}': CPU layout is {} byte(s) per pixel ({} channels of {} byte(s)) "
                 "but the image format needs {}. The texture stays empty.",
                 m_vulkanTexture->getDebugName(), dstPixel, dstChannels, channelSize, texelSize);
        return;
    }

    auto staging = device->createStagingBuffer(pixelCount * dstPixel, "texture_upload");
    if(!staging) return;

    auto* dst = static_cast<std::uint8_t*>(staging->getMappedPointer());
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
        if(m_dataType == SGGDataType::SGG_FLOAT)
        {
            const float one = 1.0f;
            for(std::uint64_t i = 0; i < pixelCount; ++i) std::memcpy(dst + i * dstPixel + srcPixel, &one, sizeof(one));
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
