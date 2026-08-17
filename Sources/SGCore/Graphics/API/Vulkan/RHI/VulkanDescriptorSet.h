//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <cstdint>
#include <map>

#include <sgcore_export.h>

#include "SGCore/Graphics/RHI/IDescriptorSet.h"
#include "VulkanContext.h"

namespace SGCore
{
    class VulkanDevice;
    class VulkanShaderProgram;

    /// Binding table on the CPU side. Bound to a command list it materializes into a transient
    /// VkDescriptorSet matching the current pipeline's layout (see VulkanCommandList::flushState()).
    class SGCORE_EXPORT VulkanDescriptorSet final : public IDescriptorSet
    {
    public:
        struct Entry
        {
            Ref<IGPUBuffer> m_buffer;
            std::uint64_t m_offset { };
            std::uint64_t m_range { };
            bool m_storage { };
            Ref<ITexture2D> m_texture;
            Ref<ICubemapTexture> m_cubemap;
            std::uint32_t m_arrayIndex { };
        };

        explicit VulkanDescriptorSet(VulkanDevice& device) noexcept : m_device(device) { }

        void setUniformBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
                              std::uint64_t offset = 0, std::uint64_t range = 0) noexcept override;
        void setStorageBuffer(std::uint32_t binding, const Ref<IGPUBuffer>& buffer,
                              std::uint64_t offset = 0, std::uint64_t range = 0) noexcept override;
        void setTexture(std::uint32_t binding, const Ref<ITexture2D>& texture, std::uint32_t arrayIndex = 0) noexcept override;
        void setCubemap(std::uint32_t binding, const Ref<ICubemapTexture>& texture) noexcept override;

        /// (binding, arrayIndex) -> entry
        [[nodiscard]] const std::map<std::pair<std::uint32_t, std::uint32_t>, Entry>& getEntries() const noexcept { return m_entries; }
        [[nodiscard]] std::uint64_t getVersion() const noexcept { return m_version; }

    private:
        VulkanDevice& m_device;
        std::map<std::pair<std::uint32_t, std::uint32_t>, Entry> m_entries;
        std::uint64_t m_version { };
    };
}
