//
// Created by 8bitniksis on 18.08.2026.
//

#pragma once

#if defined(_WIN32)

#include <memory>

#include "SGCore/Graphics/API/IRenderer.h"
#include "RHI/DX12Context.h"

namespace SGCore
{
    /**
     * The DX12 renderer (stage 3 of docs/IMPLEMENTATION_PLAN.md).
     *
     * Currently brings up the device: adapter selection, D3D12 device, direct queue and the debug
     * layer. The RHI implementation (DX12Device/CommandList/PipelineState) and the legacy Vk*-style
     * facades follow in tasks 3.3 and 3.4; until then the IRenderer factories return nullptr and the
     * backend stays out of GAPISelector's default order, exactly as the Vulkan backend did while it
     * was being built.
     */
    class DX12Renderer : public IRenderer
    {
    private:
        DX12Renderer() noexcept = default;

        static inline std::shared_ptr<DX12Renderer> m_instance;

    public:
        DX12Renderer(const DX12Renderer&) = delete;
        DX12Renderer(DX12Renderer&&) = delete;
        ~DX12Renderer() override;

        void init() noexcept override;
        bool confirmSupport() noexcept final;

        void prepareFrame(const glm::ivec2& windowSize) override;
        void printInfo() noexcept override;
        void checkForErrors(const std::source_location& location = std::source_location::current()) noexcept override;

        void renderMeshData(const IMeshData* meshData, const MeshRenderState& meshRenderState) override;
        void renderArray(const Ref<IVertexArray>& vertexArray, const MeshRenderState& meshRenderState,
                         const int& verticesCount, const int& indicesCount) override;
        void renderArrayInstanced(const Ref<IVertexArray>& vertexArray, const MeshRenderState& meshRenderState,
                                  const int& verticesCount, const int& indicesCount, const int& instancesCount) override;

        void useState(const RenderState& newRenderState, bool forceState = false) noexcept final;
        void useBlendingState(const BlendingState& newBlendingState, bool forceState = false) noexcept final;
        void useMeshRenderState(const MeshRenderState& newMeshRenderState, bool forceState = false) noexcept final;

        // ---- resource factories: nullptr until task 3.3 backs them with the RHI
        [[nodiscard]] IShader* createShader() override;
        [[nodiscard]] IVertexArray* createVertexArray() override;
        [[nodiscard]] IVertexBuffer* createVertexBuffer() override;
        [[nodiscard]] IIndexBuffer* createIndexBuffer() override;
        [[nodiscard]] ITexture2D* createTexture2D() override;
        [[nodiscard]] ICubemapTexture* createCubemapTexture() override;
        [[nodiscard]] IUniformBuffer* createUniformBuffer() override;
        [[nodiscard]] IFrameBuffer* createFrameBuffer() override;
        [[nodiscard]] IMeshData* createMeshData() const override;

        void bindScreenFrameBuffer() const noexcept final;
        void setViewport(int x, int y, int width, int height) const noexcept final;

        IGPUObjectsStorage& storage() noexcept final;
        const IGPUObjectsStorage& storage() const noexcept final;

        void reload() noexcept override;
        void shutdown() noexcept override;

        [[nodiscard]] DX12Context& getContext() noexcept { return *m_context; }

        static const std::shared_ptr<DX12Renderer>& getInstance() noexcept;

    private:
        std::shared_ptr<DX12Context> m_context = std::make_shared<DX12Context>();

        /// Logged once so a run on the unfinished backend says why nothing is drawn instead of
        /// silently producing an empty frame.
        void reportUnimplemented(const char* what) const noexcept;
    };
}

#endif
