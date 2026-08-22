//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <cstdint>

#include "SGCore/Graphics/API/GraphicsDataTypes.h"
#include "SGCore/Graphics/API/PixelConversion.h"
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

        /// Channel layout of a plain VkFormat, the input of PixelConversion: image<->CPU transfers
        /// are raw memory copies here (unlike glReadPixels / glTexImage, which convert), so anything
        /// moving pixels in or out in a different data type has to convert them itself.
        /// Packed, compressed and planar formats report UNKNOWN: their texels have no channel array.
        [[nodiscard]] static ChannelLayout formatLayout(VkFormat format) noexcept;

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
