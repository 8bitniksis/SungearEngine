//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <glad/glad.h>

#include "SGCore/Graphics/RHI/IShaderProgram.h"

namespace SGCore
{
    /// Linked GL program built from GLSL stage sources (SGSLEVulkanizer OPENGL dialect).
    /// The reflection is queried from the program itself (GL_UNIFORM_BLOCK / GL_UNIFORM /
    /// GL_PROGRAM_INPUT interfaces) — the driver's view is the ground truth on GL.
    class GL46ShaderProgram final : public IShaderProgram
    {
    public:
        explicit GL46ShaderProgram(const ShaderProgramDesc& desc) noexcept;
        ~GL46ShaderProgram() override;

        [[nodiscard]] bool isValid() const noexcept override { return m_program != 0; }
        [[nodiscard]] GLuint getHandle() const noexcept { return m_program; }

        /// Queries the program interface of a linked GL program (uniform blocks with members and
        /// per-element array strides in BlockMember::m_paddedSize, samplers, vertex inputs).
        static void reflectProgram(GLuint program, ShaderReflection& out) noexcept;

    private:
        GLuint m_program { };

        [[nodiscard]] GLuint compileStage(const ShaderStageSource& stage) noexcept;
        void reflect() noexcept;
    };
}
