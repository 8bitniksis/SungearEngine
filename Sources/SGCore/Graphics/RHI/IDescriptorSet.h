//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <cstdint>
#include <sgcore_export.h>

#include "IGPUBuffer.h"
#include "IGPUObject.h"
#include "SGCore/Main/CoreGlobals.h"

namespace SGCore
{
    class ITexture2D;
    class ICubemapTexture;

    /**
     * A set of resource bindings (uniform buffers, samplers) addressed by binding number — the
     * numbers come from ShaderReflection / SGSLEVulkanizer, never hardcoded in a pass. On GL a
     * descriptor set is a recorded list of glBindBufferBase / glBindTextureUnit calls replayed
     * at bind time; on Vulkan/DX12 it maps to a descriptor set / descriptor table.
     */
    class SGCORE_EXPORT IDescriptorSet : public IGPUObject
    {
    public:
        virtual void setUniformBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
                                      std::uint64_t offset = 0, std::uint64_t range = 0) noexcept = 0;
        virtual void setStorageBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
                                      std::uint64_t offset = 0, std::uint64_t range = 0) noexcept = 0;
        /// Textures stay legacy ITexture2D objects until they move under the RHI.
        virtual void setTexture(std::uint32_t binding, const Ref<ITexture2D>& texture, std::uint32_t arrayIndex = 0) noexcept = 0;
        virtual void setCubemap(std::uint32_t binding, const Ref<ICubemapTexture>& texture) noexcept = 0;
    };
}
