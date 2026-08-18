//
// Created by 8bitniksis on 18.08.2026.
//

#include "DX12Renderer.h"

#if defined(_WIN32)

#include <cstdlib>
#include <set>
#include <string>

#include "SGCore/Graphics/API/IGPUObjectsStorage.h"
#include "SGCore/Logger/Logger.h"
#include "SGCore/Main/CoreMain.h"

namespace
{
    /// Nothing to recreate on DX12: GPU objects do not die with the window (no context), same as on
    /// Vulkan.
    struct DX12ObjectsStorage final : SGCore::IGPUObjectsStorage
    {
        void recreateAll() noexcept override { }
        void clear() noexcept override { }
    };

    DX12ObjectsStorage s_storage;
}

SGCore::IGPUObjectsStorage& SGCore::DX12Renderer::storage() noexcept
{
    return s_storage;
}

const SGCore::IGPUObjectsStorage& SGCore::DX12Renderer::storage() const noexcept
{
    return s_storage;
}

const std::shared_ptr<SGCore::DX12Renderer>& SGCore::DX12Renderer::getInstance() noexcept
{
    static std::shared_ptr<DX12Renderer> s_instancePointer(new DX12Renderer);
    s_instancePointer->m_apiType = SG_API_TYPE_DX12;

    return s_instancePointer;
}

SGCore::DX12Renderer::~DX12Renderer()
{
    DX12Renderer::shutdown();
}

bool SGCore::DX12Renderer::confirmSupport() noexcept
{
#ifdef SUNGEAR_DEBUG
    bool enableDebugLayer = true;
#else
    bool enableDebugLayer = false;
#endif
    // SG_DX_VALIDATION=1/0 overrides the build default, the counterpart of SG_VK_VALIDATION
    if(const char* forced = std::getenv("SG_DX_VALIDATION"); forced && forced[0] != 0)
    {
        enableDebugLayer = *forced != '0';
        SG_LOG_I("DX12: debug layer {} by SG_DX_VALIDATION.", enableDebugLayer ? "forced on" : "forced off");
    }

    if(!m_context->create(enableDebugLayer))
    {
        m_context->destroy();
        return false;
    }

    return true;
}

void SGCore::DX12Renderer::init() noexcept
{
    // every backend runs confirmSupport() from its own init(), as GL4Renderer and VkRenderer do
    if(!confirmSupport())
    {
        SG_LOG_C("DX12 renderer failed to initialize. Closing the window.");
        CoreMain::getWindow().setShouldClose(true);
        return;
    }

    printInfo();

    // The device is up, but nothing above it is: the RHI implementation and the legacy facades are
    // tasks 3.3/3.4. Letting the engine continue would hand it nullptr from every resource factory
    // and crash on the first dereference, so the run is stopped here with a clear reason instead.
    SG_LOG_C("DX12 backend: the device initializes, but resources and rendering are not implemented "
             "yet (stage 3, tasks 3.3/3.4). Use --gapi gl46 or --gapi vulkan to run the engine.");
    CoreMain::getWindow().setShouldClose(true);
}

void SGCore::DX12Renderer::printInfo() noexcept
{
    SG_LOG_I("-----------------------------------");
    SG_LOG_I("DirectX 12 info:");
    SG_LOG_I("Adapter: {}", m_context->getAdapterName());
    SG_LOG_I("Feature level: 12.0");
    SG_LOG_I("Debug layer: {}", m_context->m_debugLayerEnabled ? "on" : "off");
    SG_LOG_I("-----------------------------------");
}

void SGCore::DX12Renderer::checkForErrors(const std::source_location& /*location*/) noexcept
{
    // the debug layer has no callback: its queue is drained instead
    m_context->drainDebugMessages();
}

void SGCore::DX12Renderer::prepareFrame(const glm::ivec2& /*windowSize*/)
{
    // swapchain arrives with task 3.2's presentation half
}

void SGCore::DX12Renderer::shutdown() noexcept
{
    m_context->destroy();
}

void SGCore::DX12Renderer::reload() noexcept
{
}

void SGCore::DX12Renderer::reportUnimplemented(const char* what) const noexcept
{
    static std::set<std::string> s_reported;
    if(!s_reported.insert(what).second) return;

    SG_LOG_W("DX12: '{}' is not implemented yet (stage 3, tasks 3.3/3.4). Nothing will be drawn through it.", what);
}

// ---- resource factories: nullptr until the RHI implementation lands (task 3.3)

SGCore::IShader* SGCore::DX12Renderer::createShader() { reportUnimplemented("createShader"); return nullptr; }
SGCore::IVertexArray* SGCore::DX12Renderer::createVertexArray() { reportUnimplemented("createVertexArray"); return nullptr; }
SGCore::IVertexBuffer* SGCore::DX12Renderer::createVertexBuffer() { reportUnimplemented("createVertexBuffer"); return nullptr; }
SGCore::IIndexBuffer* SGCore::DX12Renderer::createIndexBuffer() { reportUnimplemented("createIndexBuffer"); return nullptr; }
SGCore::ITexture2D* SGCore::DX12Renderer::createTexture2D() { reportUnimplemented("createTexture2D"); return nullptr; }
SGCore::ICubemapTexture* SGCore::DX12Renderer::createCubemapTexture() { reportUnimplemented("createCubemapTexture"); return nullptr; }
SGCore::IUniformBuffer* SGCore::DX12Renderer::createUniformBuffer() { reportUnimplemented("createUniformBuffer"); return nullptr; }
SGCore::IFrameBuffer* SGCore::DX12Renderer::createFrameBuffer() { reportUnimplemented("createFrameBuffer"); return nullptr; }
SGCore::IMeshData* SGCore::DX12Renderer::createMeshData() const { reportUnimplemented("createMeshData"); return nullptr; }

void SGCore::DX12Renderer::renderMeshData(const IMeshData*, const MeshRenderState&) { reportUnimplemented("renderMeshData"); }
void SGCore::DX12Renderer::renderArray(const Ref<IVertexArray>&, const MeshRenderState&, const int&, const int&) { reportUnimplemented("renderArray"); }
void SGCore::DX12Renderer::renderArrayInstanced(const Ref<IVertexArray>&, const MeshRenderState&, const int&, const int&, const int&) { reportUnimplemented("renderArrayInstanced"); }

void SGCore::DX12Renderer::useState(const RenderState&, bool) noexcept { }
void SGCore::DX12Renderer::useBlendingState(const BlendingState&, bool) noexcept { }
void SGCore::DX12Renderer::useMeshRenderState(const MeshRenderState&, bool) noexcept { }

void SGCore::DX12Renderer::bindScreenFrameBuffer() const noexcept { }
void SGCore::DX12Renderer::setViewport(int, int, int, int) const noexcept { }

#endif
