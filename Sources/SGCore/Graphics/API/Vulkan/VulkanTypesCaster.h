//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <cstdint>

#include "SGCore/Graphics/API/GraphicsDataTypes.h"
#include "VulkanCommon.h"

namespace SGCore
{
    /// Engine enum -> Vulkan enum conversions. Mirrors GLGraphicsTypesCaster for the GL backend.
    struct VulkanTypesCaster
    {
        /// Image format of a texture / attachment. Formats Vulkan implementations rarely support
        /// (packed RGB8, RGB16...) are widened to their RGBA counterparts; the caller must expand the
        /// upload data accordingly (see needsAlphaExpansion()).
        [[nodiscard]] static VkFormat sggInternalFormatToVk(SGGColorInternalFormat format) noexcept;

        /// True when sggInternalFormatToVk() widened a 3-channel format to 4 channels.
        [[nodiscard]] static bool needsAlphaExpansion(SGGColorInternalFormat format) noexcept;

        /// Bytes one texel of a VkFormat occupies. Buffer<->image copies are sized by the image
        /// format, not by the CPU-side channel count, so uploads must use this.
        /// 0 for formats without a plain byte size (compressed, planar).
        [[nodiscard]] static std::uint32_t formatTexelSize(VkFormat format) noexcept;

        /// Numeric kind of one channel of a plain VkFormat.
        enum class FormatChannelKind
        {
            UNKNOWN,
            UNORM,
            SNORM,
            UINT,
            SINT,
            SFLOAT
        };

        /// How a plain VkFormat stores one texel, channel by channel.
        struct FormatLayout
        {
            FormatChannelKind m_kind = FormatChannelKind::UNKNOWN;
            std::uint32_t m_channels = 0;
            /// Bytes one channel occupies; 0 together with UNKNOWN.
            std::uint32_t m_channelSize = 0;
            /// True for the B8G8R8A8 family: channel 0 in memory is blue, not red.
            bool m_reversedChannels = false;
        };

        /// Channel layout of a plain VkFormat. Image<->CPU transfers are raw memory copies on Vulkan
        /// (unlike glReadPixels / glTexImage, which convert), so anything reading an image into a
        /// different data type has to decode it through this.
        /// Packed, compressed and planar formats report UNKNOWN: their texels have no channel array.
        [[nodiscard]] static FormatLayout formatLayout(VkFormat format) noexcept;

        [[nodiscard]] static bool isDepthFormat(SGGColorInternalFormat format) noexcept;
        [[nodiscard]] static bool isDepthStencilFormat(SGGColorInternalFormat format) noexcept;

        /// Vertex attribute format from the engine's (type, components, normalized) triple.
        [[nodiscard]] static VkFormat vertexAttributeFormat(SGGDataType type, std::uint32_t components, bool normalized) noexcept;

        [[nodiscard]] static VkCompareOp sggCompareToVk(SGDepthStencilFunc func) noexcept;
        [[nodiscard]] static VkStencilOp sggStencilOpToVk(SGStencilOp op) noexcept;
        [[nodiscard]] static VkBlendFactor sggBlendFactorToVk(SGBlendingFactor factor) noexcept;
        [[nodiscard]] static VkBlendOp sggBlendEquationToVk(SGEquation equation) noexcept;
        [[nodiscard]] static VkCullModeFlags sggFaceTypeToVk(SGFaceType faceType) noexcept;
        [[nodiscard]] static VkPrimitiveTopology sggDrawModeToVk(SGDrawMode drawMode) noexcept;
    };
}
