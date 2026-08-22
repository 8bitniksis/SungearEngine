//
// Created by 8bitniksis on 20.08.2026.
//

#include "DX12DescriptorSet.h"

#if defined(_WIN32)

void SGCore::DX12DescriptorSet::setUniformBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
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

void SGCore::DX12DescriptorSet::setStorageBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
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

void SGCore::DX12DescriptorSet::setTexture(std::uint32_t binding, const Ref<ITexture2D>& texture, std::uint32_t arrayIndex) noexcept
{
    auto& entry = m_entries[{ binding, arrayIndex }];
    entry = { };
    entry.m_texture = texture;
    entry.m_arrayIndex = arrayIndex;
    ++m_version;
}

void SGCore::DX12DescriptorSet::setBackendTexture(std::uint32_t binding, const Ref<IGPUObject>& texture, std::uint32_t arrayIndex) noexcept
{
    // the unit table keeps textures as IGPUObject; on this backend they are always DX12Texture
    setDX12Texture(binding, std::static_pointer_cast<DX12Texture>(texture), arrayIndex);
}

void SGCore::DX12DescriptorSet::setDX12Texture(std::uint32_t binding, const Ref<DX12Texture>& texture, std::uint32_t arrayIndex) noexcept
{
    auto& entry = m_entries[{ binding, arrayIndex }];
    // re-binding the same texture every frame must not invalidate the set, or every draw would
    // allocate a fresh descriptor table
    if(entry.m_dx12Texture == texture && !entry.m_texture && !entry.m_buffer) return;

    entry = { };
    entry.m_dx12Texture = texture;
    entry.m_arrayIndex = arrayIndex;
    ++m_version;
}

void SGCore::DX12DescriptorSet::setCubemap(std::uint32_t binding, const Ref<ICubemapTexture>& texture) noexcept
{
    auto& entry = m_entries[{ binding, 0 }];
    entry = { };
    entry.m_cubemap = texture;
    ++m_version;
}

#endif
