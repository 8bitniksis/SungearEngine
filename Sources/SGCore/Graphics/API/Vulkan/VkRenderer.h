//
// Created by stuka on 07.07.2023.
//

#ifndef SUNGEARENGINE_VKRENDERER_H
#define SUNGEARENGINE_VKRENDERER_H

#include <memory>

#include "SGCore/Graphics/API/IRenderer.h"
#include "VkShader.h"
#include "VkVertexArray.h"
#include "VkVertexBuffer.h"
#include "VkIndexBuffer.h"
#include "VkTexture2D.h"
#include "VkUniformBuffer.h"
#include "VkMeshData.h"
#include "VkFrameBuffer.h"
#include "VkCubemapTexture.h"
#include "RHI/VulkanContext.h"

namespace SGCore
{
    class VulkanDevice;
    class ScreenBlit;

    /// The Vulkan renderer: owns the VulkanContext (instance/device) and the RHI VulkanDevice.
    /// Legacy IRenderer factories return Vk* facades; those not yet backed by the RHI (shaders,
    /// vertex arrays, uniform buffers...) are still no-op stubs, see docs/IMPLEMENTATION_PLAN.md 2.3.
    class VkRenderer : public IRenderer
    {
    private:
        VkRenderer() noexcept = default;

        // singleton instance
        static inline std::shared_ptr<VkRenderer> m_instance;

    public:
        VkRenderer(const VkRenderer&) = delete;
        VkRenderer(VkRenderer&&) = delete;
        ~VkRenderer() override;

        void init() noexcept override;

        bool confirmSupport() noexcept final;

        void prepareFrame(const glm::ivec2& windowSize) override;

        void useState(const RenderState& newRenderState, bool forceState = false) noexcept final;
        void useBlendingState(const BlendingState& newBlendingState, bool forceState = false) noexcept final;
        void useMeshRenderState(const MeshRenderState& newMeshRenderState, bool forceState = false) noexcept final;

        void printInfo() noexcept override;

        /**
         * Checks for errors in GAPI.
         * @param location - Where the function is called from.
         */
        void checkForErrors(const std::source_location& location = std::source_location::current()) noexcept override;

        [[nodiscard]] VkShader* createShader() override;
        [[nodiscard]] VkVertexArray* createVertexArray() override;
        [[nodiscard]] VkVertexBuffer* createVertexBuffer() override;
        [[nodiscard]] VkIndexBuffer* createIndexBuffer() override;
        [[nodiscard]] VkTexture2D* createTexture2D() override;
        [[nodiscard]] VkCubemapTexture* createCubemapTexture() override;
        [[nodiscard]] VkUniformBuffer* createUniformBuffer() override;
        [[nodiscard]] VkFrameBuffer* createFrameBuffer() override;

        [[nodiscard]] VkMeshData* createMeshData() const override;

        void bindScreenFrameBuffer() const noexcept final;
        void setViewport(int x, int y, int width, int height) const noexcept final;

        void renderTextureOnScreen(const ITexture2D* texture, bool flipOutput, int x, int y, int width, int height) noexcept override;
        [[nodiscard]] bool readScreenPixels(AttachmentReadback& out) const noexcept override;

        IGPUObjectsStorage& storage() noexcept final;
        const IGPUObjectsStorage& storage() const noexcept final;

        void reload() noexcept override;
        void shutdown() noexcept override;

        [[nodiscard]] IDevice* getDevice() noexcept override;
        [[nodiscard]] VulkanDevice* getVulkanDevice() const noexcept { return m_device.get(); }
        [[nodiscard]] VulkanContext& getContext() noexcept { return *m_context; }

        /// The command list legacy VkFrameBuffer::bind()/unbind() record into (one per renderer).
        [[nodiscard]] ICommandList* getFrameBufferCommandList() noexcept;
        [[nodiscard]] const Ref<ICommandList>& getFrameBufferCommandListRef() noexcept;

        static const std::shared_ptr<VkRenderer>& getInstance() noexcept;

        /// The live device, or nullptr once shutdown() ran (or before init()). Legacy facades must
        /// use this instead of getInstance()->getVulkanDevice(): assets outlive the renderer and are
        /// destroyed in static destruction, when the singleton itself may already be gone.
        [[nodiscard]] static VulkanDevice* getLiveDevice() noexcept;

    private:
        std::shared_ptr<VulkanContext> m_context = std::make_shared<VulkanContext>();
        std::unique_ptr<VulkanDevice> m_device;
        Ref<ICommandList> m_frameBufferCommandList;

        std::unique_ptr<ScreenBlit> m_screenBlit;
        bool m_screenBlitInitTried { };

        /// Not a member of the singleton on purpose: it must stay readable after the singleton dies.
        static inline VulkanDevice* s_liveDevice = nullptr;

    };
}

#endif //SUNGEARENGINE_VKRENDERER_H
