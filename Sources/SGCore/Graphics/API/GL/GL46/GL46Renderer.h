//
// Created by stuka on 24.04.2023.
//

#ifndef SUNGEARENGINE_GL46RENDERER_H
#define SUNGEARENGINE_GL46RENDERER_H

#include "SGCore/Graphics/API/GL/GL46/GL46Shader.h"
#include "SGCore/Graphics/API/GL/GLVertexArray.h"
#include "SGCore/Graphics/API/GL/GL46/GL46UniformBuffer.h"

#include "SGCore/Graphics/API/IRenderer.h"
#include "SGCore/ImportedScenesArch/IMeshData.h"

#include "SGCore/Graphics/API/GL/GL4/GL4Renderer.h"

namespace SGCore
{
    class CoreMain;

    /**
     * OpenGL 4.6 backend: the GL4 implementation running on a 4.6 core context with
     * `#version 460 core` shaders. Direct-state-access paths are introduced per object type
     * during the RHI migration, once each is covered by the smoke test.
     */
    class GL46Renderer : public GL4Renderer
    {
    public:
        GL46Renderer(const GL46Renderer&) = delete;
        GL46Renderer(GL46Renderer&&) = delete;

        bool confirmSupport() noexcept override;

        [[nodiscard]] GL46Shader* createShader() override;

        static const std::shared_ptr<GL46Renderer>& getInstance() noexcept;

    private:
        GL46Renderer() noexcept = default;
    };
}

#endif //SUNGEARENGINE_GL46RENDERER_H
