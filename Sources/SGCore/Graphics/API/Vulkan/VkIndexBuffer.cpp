//
// Created by stuka on 07.07.2023.
//

#include "VkIndexBuffer.h"

#include "RHI/VulkanDevice.h"
#include "RHI/VulkanGPUBuffer.h"
#include "VkRenderer.h"

namespace
{
    SGCore::VulkanDevice* currentDevice() noexcept
    {
        return SGCore::VkRenderer::getLiveDevice();
    }
}

SGCore::VkIndexBuffer::~VkIndexBuffer() noexcept
{
    VkIndexBuffer::destroy();
}

void SGCore::VkIndexBuffer::ensureCapacity(std::uint64_t byteSize) noexcept
{
    if(byteSize == 0) return;
    if(m_gpuBuffer && m_capacity >= byteSize) return;

    auto* device = currentDevice();
    if(!device) return;

    if(m_gpuBuffer) device->destroyDeferred(m_gpuBuffer);

    GPUBufferDesc desc;
    desc.m_size = byteSize;
    desc.m_usage = GPUBufferUsage::SGG_INDEX_BUFFER;
    desc.m_access = m_usage == SGGUsage::SGG_STATIC ? GPUMemoryAccess::SGG_DEVICE_LOCAL : GPUMemoryAccess::SGG_HOST_VISIBLE;
    desc.m_debugName = "index_buffer";

    m_gpuBuffer = device->createBuffer(desc);
    m_capacity = m_gpuBuffer ? byteSize : 0;
}

void SGCore::VkIndexBuffer::create() noexcept
{
    ensureCapacity(m_data.size() * sizeof(std::uint32_t));
    if(m_gpuBuffer && !m_data.empty())
    {
        m_gpuBuffer->write(m_data.data(), m_data.size() * sizeof(std::uint32_t), 0);
    }
}

void SGCore::VkIndexBuffer::create(const size_t& byteSize) noexcept
{
    m_data.resize(byteSize / sizeof(std::uint32_t));
    ensureCapacity(byteSize);
}

void SGCore::VkIndexBuffer::destroy() noexcept
{
    if(!m_gpuBuffer) return;
    if(auto* device = currentDevice()) device->destroyDeferred(m_gpuBuffer);
    m_gpuBuffer = nullptr;
    m_capacity = 0;
}

void SGCore::VkIndexBuffer::putData(const std::vector<std::uint32_t>& data) noexcept
{
    m_data = data;
    ensureCapacity(m_data.size() * sizeof(std::uint32_t));
    if(m_gpuBuffer && !m_data.empty())
    {
        m_gpuBuffer->write(m_data.data(), m_data.size() * sizeof(std::uint32_t), 0);
    }
}

void SGCore::VkIndexBuffer::subData(const std::vector<std::uint32_t>& data, const int& offset) noexcept
{
    subData(const_cast<std::uint32_t*>(data.data()), data.size(), offset);
}

void SGCore::VkIndexBuffer::subData(std::uint32_t* data, const size_t& elementsCount, const int& offset) noexcept
{
    if(!data || elementsCount == 0 || offset < 0) return;

    const std::size_t elementOffset = static_cast<std::size_t>(offset);
    if(elementOffset + elementsCount > m_data.size()) return;
    std::memcpy(m_data.data() + elementOffset, data, elementsCount * sizeof(std::uint32_t));

    if(!m_gpuBuffer) return;
    m_gpuBuffer->write(data, elementsCount * sizeof(std::uint32_t), elementOffset * sizeof(std::uint32_t));
}

void SGCore::VkIndexBuffer::bind() noexcept
{
    // index buffers are bound per draw through ICommandList::bindIndexBuffer
}

void SGCore::VkIndexBuffer::setUsage(SGGUsage usage) noexcept
{
    m_usage = usage;
}

std::uintptr_t SGCore::VkIndexBuffer::getNativeHandle() const noexcept
{
    const auto* buffer = static_cast<const VulkanGPUBuffer*>(m_gpuBuffer.get());
    return buffer ? reinterpret_cast<std::uintptr_t>(buffer->getHandle()) : 0;
}
