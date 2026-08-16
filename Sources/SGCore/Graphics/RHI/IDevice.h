//
// Created by 8bitniksis on 17.08.2026.
//

#pragma once

#include <sgcore_export.h>

#include "ICommandList.h"
#include "IDescriptorSet.h"
#include "IGPUBuffer.h"
#include "IPipelineState.h"
#include "IShaderProgram.h"
#include "ISwapchain.h"
#include "RHITypes.h"
#include "SGCore/Main/CoreGlobals.h"

namespace SGCore
{
    /**
     * Root of the render hardware interface: creates GPU objects, owns the pipeline cache and
     * the swapchain, reports backend properties. One IDevice per process; obtained through
     * IRenderer::getDevice() during the migration (docs/RHI_DESIGN.md).
     */
    class SGCORE_EXPORT IDevice
    {
    public:
        virtual ~IDevice() = default;

        [[nodiscard]] virtual const DeviceProperties& getProperties() const noexcept = 0;

        [[nodiscard]] virtual Ref<IGPUBuffer> createBuffer(const GPUBufferDesc& desc) noexcept = 0;
        [[nodiscard]] virtual Ref<IShaderProgram> createShaderProgram(const ShaderProgramDesc& desc) noexcept = 0;
        [[nodiscard]] virtual Ref<IDescriptorSet> createDescriptorSet() noexcept = 0;

        /// Pipeline cache: an equal desc returns the same object.
        [[nodiscard]] virtual Ref<IPipelineState> getOrCreatePipeline(const PipelineStateDesc& desc) noexcept = 0;

        [[nodiscard]] virtual Ref<ICommandList> createCommandList() noexcept = 0;

        /// Executes a recorded command list. Explicit backends queue it for the frame's submit;
        /// GL has already executed it during recording and returns immediately.
        virtual void submit(const Ref<ICommandList>& commandList) noexcept = 0;

        [[nodiscard]] virtual ISwapchain& getSwapchain() noexcept = 0;

        /// Releases the object once every frame that could still use it has completed.
        virtual void destroyDeferred(Ref<IGPUObject> object) noexcept = 0;

        /// Blocks until the GPU finished all submitted work (shutdown, resource re-creation).
        virtual void waitIdle() noexcept = 0;
    };
}
