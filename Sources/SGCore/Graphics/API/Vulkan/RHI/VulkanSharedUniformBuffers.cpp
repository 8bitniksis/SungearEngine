//
// Created by 8bitniksis on 17.08.2026.
//

#include "VulkanSharedUniformBuffers.h"

void SGCore::VulkanSharedUniformBuffers::set(const std::string& blockName, Ref<IGPUBuffer> buffer) noexcept
{
    if(blockName.empty()) return;
    s_buffers[blockName] = std::move(buffer);
}

const SGCore::Ref<SGCore::IGPUBuffer>& SGCore::VulkanSharedUniformBuffers::get(const std::string& blockName) noexcept
{
    static const Ref<IGPUBuffer> empty;
    const auto it = s_buffers.find(blockName);
    return it == s_buffers.end() ? empty : it->second;
}

void SGCore::VulkanSharedUniformBuffers::remove(const std::string& blockName) noexcept
{
    s_buffers.erase(blockName);
}

void SGCore::VulkanSharedUniformBuffers::clear() noexcept
{
    s_buffers.clear();
}
