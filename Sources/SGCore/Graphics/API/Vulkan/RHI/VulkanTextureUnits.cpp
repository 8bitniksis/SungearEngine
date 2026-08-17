//
// Created by 8bitniksis on 17.08.2026.
//

#include "VulkanTextureUnits.h"

#include "VulkanTexture.h"

void SGCore::VulkanTextureUnits::set(std::uint8_t unit, Ref<VulkanTexture> texture) noexcept
{
    if(unit >= max_units) return;
    s_units[unit] = std::move(texture);
}

const SGCore::Ref<SGCore::VulkanTexture>& SGCore::VulkanTextureUnits::get(std::uint8_t unit) noexcept
{
    static const Ref<VulkanTexture> empty;
    return unit < max_units ? s_units[unit] : empty;
}

void SGCore::VulkanTextureUnits::clear() noexcept
{
    for(auto& unit : s_units) unit.reset();
}
