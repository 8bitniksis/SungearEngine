//
// Created by 8bitniksis on 17.08.2026.
//

#include "VulkanDescriptorSet.h"

void SGCore::VulkanDescriptorSet::setUniformBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
                                                   std::uint64_t offset, std::uint64_t range) noexcept
{
    auto& entry = m_entries[{ binding, 0 }];
    entry = { };
    entry.m_buffer = buffer;
    entry.m_offset = offset;
    entry.m_range = range;
    entry.m_storage = false;
    ++m_version;
}

void SGCore::VulkanDescriptorSet::setStorageBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
                                                   std::uint64_t offset, std::uint64_t range) noexcept
{
    auto& entry = m_entries[{ binding, 0 }];
    entry = { };
    entry.m_buffer = buffer;
    entry.m_offset = offset;
    entry.m_range = range;
    entry.m_storage = true;
    ++m_version;
}

void SGCore::VulkanDescriptorSet::setTexture(std::uint32_t binding, const Ref<ITexture2D>& texture, std::uint32_t arrayIndex) noexcept
{
    auto& entry = m_entries[{ binding, arrayIndex }];
    entry = { };
    entry.m_texture = texture;
    entry.m_arrayIndex = arrayIndex;
    ++m_version;
}

void SGCore::VulkanDescriptorSet::setCubemap(std::uint32_t binding, const Ref<ICubemapTexture>& texture) noexcept
{
    auto& entry = m_entries[{ binding, 0 }];
    entry = { };
    entry.m_cubemap = texture;
    ++m_version;
}
