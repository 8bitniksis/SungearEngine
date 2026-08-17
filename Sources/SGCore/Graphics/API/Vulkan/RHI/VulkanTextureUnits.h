//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <array>
#include <cstdint>

#include <sgcore_export.h>

#include "SGCore/Main/CoreGlobals.h"

namespace SGCore
{
    class VulkanTexture;

    /// Translation table between the engine's texture unit model and Vulkan descriptor sets.
    ///
    /// Render passes speak units: `shader->useTextureBlock("u_name", U)` says "sampler u_name reads
    /// unit U", and `texture->bind(U)` / `frameBuffer->bindAttachment(type, U)` says "unit U holds
    /// texture T". Vulkan has no units, so the halves are recorded separately — the sampler->unit
    /// side in VkShader, the unit->texture side here — and joined at draw time by reflection
    /// (see VkShader::buildDescriptorSet). Decision recorded in docs/RHI_DESIGN.md, textures.
    ///
    /// Global by nature: `bind()` is a method of the texture, which knows nothing about the shader
    /// that will sample it, exactly as the GL unit state it mirrors.
    struct SGCORE_EXPORT VulkanTextureUnits
    {
        /// GL guarantees at least 16 combined units; engine passes chain offsets well below 64.
        static constexpr std::uint8_t max_units = 64;

        static void set(std::uint8_t unit, Ref<VulkanTexture> texture) noexcept;
        [[nodiscard]] static const Ref<VulkanTexture>& get(std::uint8_t unit) noexcept;
        /// Drops every reference (renderer shutdown; textures must not outlive the device).
        static void clear() noexcept;

    private:
        static inline std::array<Ref<VulkanTexture>, max_units> s_units { };
    };
}
