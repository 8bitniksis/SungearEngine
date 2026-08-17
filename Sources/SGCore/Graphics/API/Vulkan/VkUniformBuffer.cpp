//
// Created by stuka on 07.07.2023.
//

#include "VkUniformBuffer.h"

#include "RHI/VulkanDevice.h"
#include "RHI/VulkanSharedUniformBuffers.h"
#include "SGCore/Logger/Logger.h"
#include "VkRenderer.h"

namespace
{
    SGCore::VulkanDevice* currentDevice() noexcept
    {
        return SGCore::VkRenderer::getLiveDevice();
    }
}

SGCore::VkUniformBuffer::~VkUniformBuffer()
{
    VkUniformBuffer::destroy();
}

void SGCore::VkUniformBuffer::prepare() noexcept
{
    destroy();

    auto* device = currentDevice();
    if(!device || m_bufferSize <= 0) return;

    GPUBufferDesc desc;
    desc.m_size = static_cast<std::uint64_t>(m_bufferSize);
    desc.m_usage = GPUBufferUsage::SGG_UNIFORM_BUFFER;
    // rewritten every frame (camera matrices, time): host-visible, no staging round trip
    desc.m_access = GPUMemoryAccess::SGG_HOST_VISIBLE;
    desc.m_debugName = m_blockName.empty() ? "shared_ubo" : m_blockName;
    m_buffer_gpu = device->createBuffer(desc);
    if(!m_buffer_gpu) return;

    if(m_buffer) m_buffer_gpu->write(m_buffer, static_cast<std::uint64_t>(m_bufferSize), 0);

    if(m_blockName.empty())
    {
        SG_LOG_W("VkUniformBuffer without a block name: shaders can not find it (bindings are assigned by name on Vulkan).");
        return;
    }
    VulkanSharedUniformBuffers::set(m_blockName, m_buffer_gpu);
}

void SGCore::VkUniformBuffer::subDataOnGAPISide(const std::int64_t& offset, const int& size) noexcept
{
    if(!m_buffer_gpu || !m_buffer || size <= 0) return;
    m_buffer_gpu->write(m_buffer + offset, static_cast<std::uint64_t>(size), static_cast<std::uint64_t>(offset));
}

void SGCore::VkUniformBuffer::bind() noexcept
{
    // nothing to bind: shaders pick the buffer up by block name when they build their descriptor set
}

void SGCore::VkUniformBuffer::setLayoutLocation(const std::uint16_t& location) noexcept
{
    m_layoutLocation = location;
}

void SGCore::VkUniformBuffer::destroy() noexcept
{
    if(!m_buffer_gpu) return;

    if(!m_blockName.empty() && VulkanSharedUniformBuffers::get(m_blockName) == m_buffer_gpu)
    {
        VulkanSharedUniformBuffers::remove(m_blockName);
    }
    if(auto* device = currentDevice()) device->destroyDeferred(m_buffer_gpu);
    m_buffer_gpu = nullptr;
}
