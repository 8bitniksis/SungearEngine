//
// Created by 8bitniksis on 17.08.2026.
//

#include "GL46CommandList.h"

#include "GL46DescriptorSet.h"
#include "GL46GPUBuffer.h"
#include "SGCore/Graphics/API/GL/GL4/GL4Renderer.h"
#include "SGCore/Graphics/API/IFrameBuffer.h"
#include "SGCore/Logger/Logger.h"

SGCore::GL46CommandList::GL46CommandList(GL4Renderer& renderer) noexcept : m_renderer(renderer)
{
}

void SGCore::GL46CommandList::begin() noexcept
{
    m_pipeline = nullptr;
}

void SGCore::GL46CommandList::end() noexcept
{
}

void SGCore::GL46CommandList::beginRenderPass(const RenderPassBeginDesc& desc) noexcept
{
    if(desc.m_frameBuffer)
    {
        desc.m_frameBuffer->bind();
        if(!desc.m_colorAttachments.empty())
        {
            desc.m_frameBuffer->bindAttachmentsToDrawIn(desc.m_colorAttachments);
        }
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if(desc.m_width > 0 && desc.m_height > 0)
    {
        glViewport(0, 0, desc.m_width, desc.m_height);
    }

    GLbitfield clearMask = 0;
    if(desc.m_colorLoadOp == LoadOp::SGG_CLEAR)
    {
        glClearColor(desc.m_clearColor.r, desc.m_clearColor.g, desc.m_clearColor.b, desc.m_clearColor.a);
        clearMask |= GL_COLOR_BUFFER_BIT;
    }
    if(desc.m_depthLoadOp == LoadOp::SGG_CLEAR)
    {
        glClearDepth(desc.m_clearDepth);
        // depth writes must be on for the clear to reach the buffer
        glDepthMask(GL_TRUE);
        clearMask |= GL_DEPTH_BUFFER_BIT;
    }
    if(clearMask) glClear(clearMask);
}

void SGCore::GL46CommandList::endRenderPass() noexcept
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SGCore::GL46CommandList::bindPipeline(const Ref<IPipelineState>& pipeline) noexcept
{
    m_pipeline = std::static_pointer_cast<GL46PipelineState>(pipeline);
    if(!m_pipeline) return;

    const auto& desc = m_pipeline->getDesc();
    m_renderer.useState(desc.m_renderState);
    m_renderer.useBlendingState(desc.m_blendingState);
    m_renderer.useMeshRenderState(desc.m_meshRenderState);

    glUseProgram(m_pipeline->getProgram());
    glBindVertexArray(m_pipeline->getVertexArray());
}

void SGCore::GL46CommandList::bindDescriptorSet(std::uint32_t /*setIndex*/, const Ref<IDescriptorSet>& set) noexcept
{
    // GL has a single flat binding space; set numbers are meaningful only for explicit APIs
    if(const auto* glSet = static_cast<const GL46DescriptorSet*>(set.get())) glSet->apply();
}

void SGCore::GL46CommandList::pushConstants(const void* /*data*/, std::uint32_t /*size*/, std::uint32_t /*offset*/) noexcept
{
    if(!m_pushConstantsWarned)
    {
        SG_LOG_W("GL46CommandList: push constants are not supported on GL yet; use a small uniform buffer.");
        m_pushConstantsWarned = true;
    }
}

void SGCore::GL46CommandList::bindVertexBuffer(std::uint32_t slot, const Ref<IGPUBuffer>& buffer, std::uint64_t offset) noexcept
{
    if(!m_pipeline)
    {
        SG_LOG_E("GL46CommandList: bindVertexBuffer before bindPipeline — the vertex layout lives in the pipeline.");
        return;
    }
    const auto* glBuffer = static_cast<const GL46GPUBuffer*>(buffer.get());
    glVertexArrayVertexBuffer(m_pipeline->getVertexArray(), slot, glBuffer ? glBuffer->getHandle() : 0,
                              static_cast<GLintptr>(offset), static_cast<GLsizei>(m_pipeline->getSlotStride(slot)));
}

void SGCore::GL46CommandList::bindIndexBuffer(const Ref<IGPUBuffer>& buffer, SGIndexType indexType, std::uint64_t offset) noexcept
{
    if(!m_pipeline)
    {
        SG_LOG_E("GL46CommandList: bindIndexBuffer before bindPipeline.");
        return;
    }
    const auto* glBuffer = static_cast<const GL46GPUBuffer*>(buffer.get());
    glVertexArrayElementBuffer(m_pipeline->getVertexArray(), glBuffer ? glBuffer->getHandle() : 0);
    m_indexType = indexType == SGIndexType::SGG_UINT16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    m_indexSize = indexType == SGIndexType::SGG_UINT16 ? 2 : 4;
    m_indexOffset = offset;
}

void SGCore::GL46CommandList::setViewport(const Viewport& viewport) noexcept
{
    glViewport(static_cast<GLint>(viewport.m_x), static_cast<GLint>(viewport.m_y),
               static_cast<GLsizei>(viewport.m_width), static_cast<GLsizei>(viewport.m_height));
    glDepthRange(viewport.m_minDepth, viewport.m_maxDepth);
}

void SGCore::GL46CommandList::setScissor(const Scissor& scissor) noexcept
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(scissor.m_x, scissor.m_y, scissor.m_width, scissor.m_height);
}

void SGCore::GL46CommandList::draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex, std::uint32_t firstInstance) noexcept
{
    if(!m_pipeline) return;
    glDrawArraysInstancedBaseInstance(m_pipeline->getDrawMode(), static_cast<GLint>(firstVertex),
                                      static_cast<GLsizei>(vertexCount), static_cast<GLsizei>(instanceCount), firstInstance);
}

void SGCore::GL46CommandList::drawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex, std::int32_t vertexOffset, std::uint32_t firstInstance) noexcept
{
    if(!m_pipeline) return;
    const auto* indices = reinterpret_cast<const void*>(static_cast<std::uintptr_t>(m_indexOffset + std::uint64_t(firstIndex) * m_indexSize));
    glDrawElementsInstancedBaseVertexBaseInstance(m_pipeline->getDrawMode(), static_cast<GLsizei>(indexCount), m_indexType, indices,
                                                  static_cast<GLsizei>(instanceCount), vertexOffset, firstInstance);
}

void SGCore::GL46CommandList::transition(const Ref<IGPUObject>& /*resource*/, GPUResourceState /*newState*/) noexcept
{
    // GL tracks hazards itself; see DeviceProperties::m_supportsExplicitBarriers
}

void SGCore::GL46CommandList::uploadData(const Ref<IGPUBuffer>& dst, const void* data, std::uint64_t size, std::uint64_t offset) noexcept
{
    if(auto* glBuffer = static_cast<GL46GPUBuffer*>(dst.get()))
    {
        glNamedBufferSubData(glBuffer->getHandle(), static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
    }
}
