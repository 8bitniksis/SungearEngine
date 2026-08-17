//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include <sgcore_export.h>

#include "SGCore/Graphics/RHI/IDevice.h"
#include "VulkanContext.h"
#include "VulkanSwapchain.h"

struct GLFWwindow;

namespace SGCore
{
    class VulkanGPUBuffer;
    class VulkanPipelineState;

    /// A transient descriptor set: allocated per bind, returned to its pool when the submission that
    /// used it retires.
    struct VulkanTransientDescriptorSet
    {
        VkDescriptorPool m_pool = VK_NULL_HANDLE;
        VkDescriptorSet m_set = VK_NULL_HANDLE;
    };

    /// One vkQueueSubmit2 worth of work plus everything that must stay alive until its fence signals.
    struct VulkanSubmission
    {
        std::vector<VkCommandBuffer> m_commandBuffers;
        std::vector<VkSemaphore> m_waitSemaphores;
        std::vector<VkPipelineStageFlags2> m_waitStages;
        std::vector<VkSemaphore> m_signalSemaphores;
        std::vector<Ref<IGPUObject>> m_keepAlive;
        std::vector<VulkanTransientDescriptorSet> m_transientSets;
    };

    /// IDevice on Vulkan 1.3 (dynamic rendering, synchronization2). Single graphics/present queue;
    /// every submit() gets its own fence, retired lazily. Owned by VkRenderer.
    class SGCORE_EXPORT VulkanDevice final : public IDevice
    {
    public:
        VulkanDevice(VulkanContext& context, GLFWwindow* window) noexcept;
        ~VulkanDevice() override;

        [[nodiscard]] bool isReady() const noexcept { return m_ready; }

        // ---- IDevice
        [[nodiscard]] const DeviceProperties& getProperties() const noexcept override { return m_properties; }
        [[nodiscard]] Ref<IGPUBuffer> createBuffer(const GPUBufferDesc& desc) noexcept override;
        [[nodiscard]] Ref<IShaderProgram> createShaderProgram(const ShaderProgramDesc& desc) noexcept override;
        [[nodiscard]] Ref<IDescriptorSet> createDescriptorSet() noexcept override;
        [[nodiscard]] Ref<IPipelineState> getOrCreatePipeline(const PipelineStateDesc& desc) noexcept override;
        [[nodiscard]] Ref<ICommandList> createCommandList() noexcept override;
        void submit(const Ref<ICommandList>& commandList) noexcept override;
        [[nodiscard]] ISwapchain& getSwapchain() noexcept override { return *m_swapchain; }
        void destroyDeferred(Ref<IGPUObject> object) noexcept override;
        void waitIdle() noexcept override;

        // ---- backend services
        [[nodiscard]] VulkanContext& getContext() noexcept { return m_context; }
        [[nodiscard]] VulkanSwapchain& getVulkanSwapchain() noexcept { return *m_swapchain; }

        /// A primary command buffer in the initial state (recycled from retired submissions).
        [[nodiscard]] VkCommandBuffer acquireCommandBuffer() noexcept;
        /// Submits recorded command buffers with a fresh fence; the submission's resources are
        /// released when the fence signals (see retire()).
        std::uint64_t submitRaw(VulkanSubmission&& submission) noexcept;
        /// Blocks until the submission with the given id (see submitRaw()) has finished; 0 = no-op.
        void waitForSubmission(std::uint64_t id) noexcept;
        /// Records into a one-shot command buffer, submits and waits. For uploads/readbacks/setup.
        void immediateSubmit(const std::function<void(VkCommandBuffer)>& record) noexcept;

        [[nodiscard]] VulkanTransientDescriptorSet allocateTransientDescriptorSet(VkDescriptorSetLayout layout) noexcept;

        /// Host-visible staging buffer of the given size (TRANSFER_SRC).
        [[nodiscard]] Ref<VulkanGPUBuffer> createStagingBuffer(std::uint64_t size, const char* debugName) noexcept;

        /// Recycles command buffers and descriptor sets of finished submissions.
        /// waitAll: block until every pending submission has finished.
        void retire(bool waitAll) noexcept;

        [[nodiscard]] std::uint64_t getSubmissionCounter() const noexcept { return m_submissionCounter; }

    private:
        struct PendingSubmission
        {
            VkFence m_fence = VK_NULL_HANDLE;
            std::uint64_t m_id { };
            VulkanSubmission m_payload;
        };

        [[nodiscard]] VkFence acquireFence() noexcept;
        [[nodiscard]] VkDescriptorPool createDescriptorPool() noexcept;

        VulkanContext& m_context;
        DeviceProperties m_properties;
        bool m_ready { };

        std::unique_ptr<VulkanSwapchain> m_swapchain;

        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_freeCommandBuffers;
        std::vector<VkFence> m_freeFences;
        std::deque<PendingSubmission> m_pending;
        std::uint64_t m_submissionCounter { };

        std::vector<VkDescriptorPool> m_descriptorPools;
        std::size_t m_currentDescriptorPool { };

        std::vector<Ref<IGPUObject>> m_deferredDestroy;

        std::unordered_map<std::size_t, std::vector<Ref<VulkanPipelineState>>> m_pipelineCache;
    };
}
