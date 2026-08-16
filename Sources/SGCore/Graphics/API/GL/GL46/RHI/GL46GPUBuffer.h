//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <glad/glad.h>

#include "SGCore/Graphics/RHI/IGPUBuffer.h"

namespace SGCore
{
    /// GL 4.6 buffer object created with direct state access; usage flags do not matter for GL
    /// (the target is chosen at bind time), only the memory access does.
    class GL46GPUBuffer final : public IGPUBuffer
    {
    public:
        explicit GL46GPUBuffer(const GPUBufferDesc& desc) noexcept;
        ~GL46GPUBuffer() override;

        [[nodiscard]] void* map(std::uint64_t offset, std::uint64_t size) noexcept override;
        void unmap() noexcept override;
        bool write(const void* data, std::uint64_t size, std::uint64_t offset) noexcept override;

        [[nodiscard]] GLuint getHandle() const noexcept { return m_handle; }

    private:
        GLuint m_handle { };
        bool m_mapped { };
    };
}
