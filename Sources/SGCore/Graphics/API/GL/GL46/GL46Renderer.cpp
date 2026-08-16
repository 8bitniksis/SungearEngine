#include <SGCore/Logger/Logger.h>
#include "GL46Renderer.h"

#include "SGCore/Memory/AssetManager.h"
#include "SGCore/Memory/Assets/TextFileAsset.h"

#include "RHI/GL46Device.h"

SGCore::GL46Renderer::~GL46Renderer() = default;

void SGCore::GL46Renderer::init() noexcept
{
    GL4Renderer::init();
    m_device = std::make_unique<GL46Device>(*this);
}

SGCore::IDevice* SGCore::GL46Renderer::getDevice() noexcept
{
    return m_device.get();
}


bool SGCore::GL46Renderer::confirmSupport() noexcept
{
    const char* versionString = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const std::string glVersion = versionString ? versionString : "";

    // "4.6.0 NVIDIA ...", "4.6 (Core Profile) Mesa ..." — only the major.minor prefix matters
    if(!glVersion.starts_with("4.6"))
    {
        SG_LOG_E("OpenGL 4.6 is not supported! Reported version: '{}'\n{}", glVersion, SG_CURRENT_LOCATION_STR);
        return false;
    }

    return true;
}

SGCore::GL46Shader* SGCore::GL46Renderer::createShader()
{
    // same shader pipeline as GL4, only the GLSL version differs: the SG_GLSL4 define selects the
    // shader variant and is mandatory — without it every stage is preprocessed away
    auto* shader = new GL46Shader;
    shader->m_version = "460 core";
    shader->addDefine(SGShaderDefineType::SGG_OTHER_DEFINE, ShaderDefine("SG_GLSL4", ""));

    m_storage.m_shaders.insert(shader);

    return shader;
}

const std::shared_ptr<SGCore::GL46Renderer>& SGCore::GL46Renderer::getInstance() noexcept
{
    static std::shared_ptr<GL46Renderer> s_instancePointer(new GL46Renderer);
    s_instancePointer->m_apiType = SG_API_TYPE_GL46;

    return s_instancePointer;
}
