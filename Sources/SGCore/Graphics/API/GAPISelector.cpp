//
// Created by 8bitniksis on 16.08.2026.
//

#include "GAPISelector.h"

#include <algorithm>
#include <cstdlib>

#include "SGCore/Logger/Logger.h"
#include "SGCore/Utils/Platform.h"

#include "SGCore/Graphics/API/GL/GL4/GL4Renderer.h"
#include "SGCore/Graphics/API/GL/GL46/GL46Renderer.h"

std::vector<SGCore::GAPIType> SGCore::GAPISelector::s_preference = SGCore::GAPISelector::getDefaultPreference();

std::vector<SGCore::GAPIType> SGCore::GAPISelector::getDefaultPreference() noexcept
{
    // Order is the design intent (see docs/RHI_DESIGN.md): explicit APIs first, OpenGL as
    // the permanent fallback. Backends that are not implemented yet are skipped at selection.
    // GL4 stays ahead of GL46 until GL46 becomes the migrated GL backend (task 1.5):
    // GL46Renderer::confirmSupport() failure currently closes the window instead of falling back.
#if SG_PLATFORM_OS_WINDOWS
    return { SG_API_TYPE_DX12, SG_API_TYPE_VULKAN, SG_API_TYPE_GL4, SG_API_TYPE_GL46 };
#elif SG_PLATFORM_OS_ANDROID
    return { SG_API_TYPE_VULKAN, SG_API_TYPE_GL4 };
#else
    return { SG_API_TYPE_VULKAN, SG_API_TYPE_GL4, SG_API_TYPE_GL46 };
#endif
}

const std::vector<SGCore::GAPIType>& SGCore::GAPISelector::getPreference() noexcept
{
    return s_preference;
}

void SGCore::GAPISelector::setPreference(std::vector<GAPIType> preference) noexcept
{
    s_preference = std::move(preference);
}

std::optional<SGCore::GAPIType> SGCore::GAPISelector::getForcedFromEnvironment() noexcept
{
    const char* forcedValue = std::getenv(forced_gapi_env_var.data());
    if(!forcedValue || *forcedValue == '\0')
    {
        return std::nullopt;
    }

    const auto forcedType = gapiTypeFromString(forcedValue);
    if(!forcedType)
    {
        SG_LOG_W("Environment variable {} holds unknown graphics API '{}'. It will be ignored.",
                 forced_gapi_env_var, forcedValue);
    }

    return forcedType;
}

SGCore::Ref<SGCore::IRenderer> SGCore::GAPISelector::createRenderer(GAPIType apiType) noexcept
{
    switch(apiType)
    {
        case SG_API_TYPE_GL4:
            return GL4Renderer::getInstance();
        case SG_API_TYPE_GL46:
            return GL46Renderer::getInstance();
        // GLES on Android is currently served by GL4Renderer under the GL4 identifier
        // (see GL4Renderer::getInstance()); GLES2/GLES3 get their own entries when that is untangled.
        case SG_API_TYPE_UNKNOWN:
        case SG_API_TYPE_GLES2:
        case SG_API_TYPE_GLES3:
        case SG_API_TYPE_VULKAN:
        case SG_API_TYPE_DX12:
            return nullptr;
    }

    return nullptr;
}

SGCore::Ref<SGCore::IRenderer> SGCore::GAPISelector::selectRenderer() noexcept
{
    std::vector<GAPIType> candidates;
    candidates.reserve(s_preference.size() + 1);

    if(const auto forcedType = getForcedFromEnvironment())
    {
        SG_LOG_I("Graphics API '{}' is forced by environment variable {}.",
                 gapiTypeToString(*forcedType), forced_gapi_env_var);
        candidates.push_back(*forcedType);
    }

    for(const auto candidate : s_preference)
    {
        if(std::find(candidates.begin(), candidates.end(), candidate) == candidates.end())
        {
            candidates.push_back(candidate);
        }
    }

    for(std::size_t i = 0; i < candidates.size(); ++i)
    {
        const auto candidate = candidates[i];
        auto renderer = createRenderer(candidate);

        if(!renderer)
        {
            SG_LOG_I("Graphics API '{}' is not available in this build. Trying next one...",
                     gapiTypeToString(candidate));
            continue;
        }

        if(i > 0)
        {
            SG_LOG_W("Graphics API '{}' selected as fallback. Preferred API '{}' is not available.",
                     gapiTypeToString(candidate), gapiTypeToString(candidates[0]));
        }
        else
        {
            SG_LOG_I("Graphics API '{}' selected.", gapiTypeToString(candidate));
        }

        return renderer;
    }

    SG_LOG_C("No graphics API from the preference list is available in this build.");

    return nullptr;
}
