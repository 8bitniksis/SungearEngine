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

#include <memory>

namespace SGCore
{
    class CoreMain;
    class GL46Device;

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

        ~GL46Renderer() override;

        void init() noexcept override;
        bool confirmSupport() noexcept override;

        [[nodiscard]] GL46Shader* createShader() override;

        [[nodiscard]] IDevice* getDevice() noexcept override;

        static const std::shared_ptr<GL46Renderer>& getInstance() noexcept;

    private:
        std::unique_ptr<GL46Device> m_device;

        GL46Renderer() noexcept = default;
    };
}

#endif //SUNGEARENGINE_GL46RENDERER_H
