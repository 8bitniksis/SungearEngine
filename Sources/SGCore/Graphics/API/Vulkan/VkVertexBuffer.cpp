//
// Created by stuka on 07.07.2023.
//

#include "VkVertexBuffer.h"

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

SGCore::VkVertexBuffer::~VkVertexBuffer() noexcept
{
    VkVertexBuffer::destroy();
}

void SGCore::VkVertexBuffer::ensureCapacity(std::uint64_t byteSize) noexcept
{
    if(byteSize == 0) return;
    if(m_gpuBuffer && m_capacity >= byteSize) return;

    auto* device = currentDevice();
    if(!device) return;

    if(m_gpuBuffer) device->destroyDeferred(m_gpuBuffer);

    GPUBufferDesc desc;
    desc.m_size = byteSize;
    desc.m_usage = GPUBufferUsage::SGG_VERTEX_BUFFER;
    // dynamic buffers (batching, debug lines, UI) are re-uploaded every frame; host-visible keeps
    // subData a memcpy instead of a staging round trip
    desc.m_access = m_usage == SGGUsage::SGG_STATIC ? GPUMemoryAccess::SGG_DEVICE_LOCAL : GPUMemoryAccess::SGG_HOST_VISIBLE;
    desc.m_debugName = "vertex_buffer";

    m_gpuBuffer = device->createBuffer(desc);
    m_capacity = m_gpuBuffer ? byteSize : 0;
}

void SGCore::VkVertexBuffer::create() noexcept
{
    ensureCapacity(m_data.size());
    if(m_gpuBuffer && !m_data.empty()) m_gpuBuffer->write(m_data.data(), m_data.size(), 0);
}

void SGCore::VkVertexBuffer::create(const size_t& byteSize) noexcept
{
    m_data.resize(byteSize);
    ensureCapacity(byteSize);
}

void SGCore::VkVertexBuffer::destroy() noexcept
{
    if(!m_gpuBuffer) return;
    if(auto* device = currentDevice()) device->destroyDeferred(m_gpuBuffer);
    m_gpuBuffer = nullptr;
    m_capacity = 0;
}

void SGCore::VkVertexBuffer::subDataOnGAPISide(const void* data, const size_t& bytesCount, const size_t& bytesOffset, bool isPutData) noexcept
{
    if(!data || bytesCount == 0) return;

    if(isPutData)
    {
        // putData() replaces the contents: the buffer may have to grow
        ensureCapacity(bytesOffset + bytesCount);
    }
    if(!m_gpuBuffer) return;

    m_gpuBuffer->write(data, bytesCount, bytesOffset);
}

void SGCore::VkVertexBuffer::bind() noexcept
{
    // vertex buffers are bound per draw through ICommandList::bindVertexBuffer
}

void SGCore::VkVertexBuffer::setUsage(SGGUsage usage) noexcept
{
    m_usage = usage;
}

void SGCore::VkVertexBuffer::addAttributeImpl(std::uint32_t, std::int32_t, SGGDataType, bool, std::int32_t, std::uint64_t, std::int32_t) noexcept
{
    // the base class already recorded the AttributeDesc; the vertex layout lives in the pipeline
}

void SGCore::VkVertexBuffer::useAttributes() const noexcept
{
    // no vertex array object on Vulkan: see addAttributeImpl
}

std::uintptr_t SGCore::VkVertexBuffer::getNativeHandle() const noexcept
{
    const auto* buffer = static_cast<const VulkanGPUBuffer*>(m_gpuBuffer.get());
    return buffer ? reinterpret_cast<std::uintptr_t>(buffer->getHandle()) : 0;
}
