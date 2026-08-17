//
// Created by 8bitniksis on 16.08.2026.
//

#include "SmokeApp.h"

#include <string>
#include <tuple>
#include <utility>
#include <glm/gtc/quaternion.hpp>

#include "ImageCompare.h"

#include "SGCore/Graphics/API/GAPIType.h"
#include "SGCore/Graphics/API/IFrameBuffer.h"
#include "SGCore/Graphics/API/IRenderer.h"
#include "SGCore/Logger/Logger.h"
#include "SGCore/Main/CoreMain.h"
#include "SGCore/Main/Window.h"
#include "SGCore/Memory/AssetManager.h"
#include "SGCore/Memory/Assets/Materials/IMaterial.h"
#include "SGCore/Memory/Assets/ModelAsset.h"
#include "SGCore/Render/Alpha/OpaqueEntityTag.h"
#include "SGCore/Render/Alpha/TransparentEntityTag.h"
#include "SGCore/Render/Atmosphere/Atmosphere.h"
#include "SGCore/Render/LayeredFrameReceiver.h"
#include "SGCore/Render/Mesh.h"
#include "SGCore/Render/ShadowMapping/CSM/CSMTarget.h"
#include "SGCore/Render/ShadowMapping/ShadowCaster.h"
#include "SGCore/Scene/Scene.h"
#include "SGCore/Transformations/Controllable3D.h"
#include "SGCore/Transformations/Transform.h"

namespace
{
    struct PlacedModel
    {
        const char* m_alias;
        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::vec4 m_color;
        float m_metallic;
        float m_roughness;
        bool m_transparent;
    };

    // Deterministic layout: a floor, three opaque PBR bodies with different metal/rough,
    // one transparent sphere. Everything sits in view of the camera placed in buildScene().
    constexpr PlacedModel placed_models[] = {
        { "cube_model",   {  0.0f, -1.0f,  0.0f }, { 12.0f, 0.2f, 12.0f }, { 0.75f, 0.75f, 0.75f, 1.0f }, 0.0f, 0.9f, false },
        { "cube_model",   { -3.0f,  0.0f, -2.0f }, {  1.0f, 1.0f,  1.0f }, { 0.85f, 0.20f, 0.15f, 1.0f }, 0.0f, 0.5f, false },
        { "sphere_model", {  0.0f,  0.5f, -2.0f }, {  1.0f, 1.0f,  1.0f }, { 0.90f, 0.85f, 0.30f, 1.0f }, 1.0f, 0.2f, false },
        { "cube_model",   {  3.0f,  0.5f, -2.0f }, {  1.0f, 1.5f,  1.0f }, { 0.20f, 0.45f, 0.90f, 1.0f }, 0.3f, 0.7f, false },
        { "sphere_model", {  0.0f,  0.5f,  1.5f }, {  1.0f, 1.0f,  1.0f }, { 0.30f, 0.90f, 0.50f, 0.45f }, 0.0f, 0.3f, true },
    };
}

SGSmoke::SmokeApp::SmokeApp(SmokeOptions options) noexcept : m_options(std::move(options))
{
}

int SGSmoke::SmokeApp::getExitCode() const noexcept
{
    return m_exitCode;
}

void SGSmoke::SmokeApp::onInit() noexcept
{
    buildScene();
}

void SGSmoke::SmokeApp::onUpdate(double dt, double fixedDt)
{
    ++m_frameIndex;

    if(!m_captured && m_frameIndex >= m_options.m_captureFrame)
    {
        captureAndFinish();
    }
}

void SGSmoke::SmokeApp::onFixedUpdate(double dt, double fixedDt)
{
}

void SGSmoke::SmokeApp::buildScene() noexcept
{
    const auto scene = SGCore::Scene::getCurrentScene();
    if(!scene)
    {
        SG_LOG_E("Smoke: BasicApp did not create a scene.");
        m_exitCode = 2;
        SGCore::CoreMain::getWindow().setShouldClose(true);
        return;
    }

    auto registry = scene->getECSRegistry();
    auto assetManager = SGCore::AssetManager::getInstance();

    // BasicApp already loaded "cube_model"; the sphere is loaded here under an alias.
    assetManager->loadAssetWithAlias<SGCore::ModelAsset>(
        "sphere_model", "${enginePath}/Resources/models/standard/sphere.obj");

    // camera: fixed pose so the reference frame is reproducible.
    // BasicApp makes the camera controllable; Controllable3D rewrites the rotation from
    // mouse deltas every frame, so it must go for the pose below to survive.
    registry->remove<SGCore::Controllable3D>(m_cameraEntity);

    if(auto* cameraTransform = registry->tryGet<SGCore::Transform>(m_cameraEntity))
    {
        cameraTransform->m_localTransform.m_position = { 0.0f, 3.0f, 8.0f };
        cameraTransform->m_localTransform.m_rotation =
            glm::angleAxis(glm::radians(-18.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    // sun shadows target on the camera
    registry->emplace<SGCore::CSMTarget>(m_cameraEntity);

    // sun: low enough to throw long shadows across the floor
    if(m_atmosphereEntity != entt::null)
    {
        if(auto* atmosphere = registry->tryGet<SGCore::Atmosphere>(m_atmosphereEntity))
        {
            atmosphere->m_sunRotation = { 0.0f, 35.0f, 40.0f };
        }
    }
    else
    {
        SG_LOG_W("Smoke: BasicApp created no atmosphere (cube_model failed to load?); frame will have no sun.");
    }

    std::size_t modelIndex = 0;
    for(const auto& placed : placed_models)
    {
        auto model = assetManager->getAsset<SGCore::ModelAsset, SGCore::AssetStorageType::BY_ALIAS>(placed.m_alias);
        if(!model)
        {
            SG_LOG_E("Smoke: model with alias '{}' is not loaded.", placed.m_alias);
            continue;
        }

        auto material = assetManager->getOrAddAssetByAlias<SGCore::IMaterial>(
            "smoke_material_" + std::to_string(modelIndex));
        material->setDiffuseColor(placed.m_color);
        material->setMetallicFactor(placed.m_metallic);
        material->setRoughnessFactor(placed.m_roughness);
        material->m_transparencyType = placed.m_transparent
                                       ? SGCore::MaterialTransparencyType::MAT_BLEND
                                       : SGCore::MaterialTransparencyType::MAT_OPAQUE;

        const auto entities = model->m_rootNode->addOnScene(scene);
        if(entities.empty()) continue;

        auto& rootTransform = registry->get<SGCore::Transform>(entities[0]);
        rootTransform.m_localTransform.m_position = placed.m_position;
        rootTransform.m_localTransform.m_scale = placed.m_scale;

        for(const auto entity : entities)
        {
            auto* mesh = registry->tryGet<SGCore::Mesh>(entity);
            if(!mesh) continue;

            mesh->m_base.setMaterial(material);
            registry->emplace<SGCore::ShadowCaster>(entity);

            // the importer tags meshes by the material they were imported with;
            // re-tag according to the material assigned here
            if(placed.m_transparent)
            {
                registry->remove<SGCore::OpaqueEntityTag>(entity);
                if(!registry->allOf<SGCore::TransparentEntityTag>(entity))
                {
                    registry->emplace<SGCore::TransparentEntityTag>(entity);
                }
            }
        }

        ++modelIndex;
    }
}

void SGSmoke::SmokeApp::captureAndFinish() noexcept
{
    m_captured = true;

    const auto scene = SGCore::Scene::getCurrentScene();
    auto* frameReceiver = scene ? scene->getECSRegistry()->tryGet<SGCore::LayeredFrameReceiver>(m_cameraEntity) : nullptr;

    if(!frameReceiver || !frameReceiver->m_layersFXFrameBuffer)
    {
        SG_LOG_E("Smoke: camera has no frame receiver to capture from.");
        m_exitCode = 2;
        SGCore::CoreMain::getWindow().setShouldClose(true);
        return;
    }

    const auto& sourceFrameBuffer = m_options.m_captureGeometryPass ? frameReceiver->m_layersFrameBuffer
                                                                    : frameReceiver->m_layersFXFrameBuffer;
    auto attachment = m_attachmentToDisplay;
    if(m_options.m_captureAttachment)
    {
        attachment = static_cast<SGFrameBufferAttachmentType>(
            std::to_underlying(SGFrameBufferAttachmentType::SGG_COLOR_ATTACHMENT0) + *m_options.m_captureAttachment);
    }
    SG_LOG_I("Smoke: capturing {} of the {} framebuffer.", sgFrameBufferAttachmentTypeToString(attachment),
             m_options.m_captureGeometryPass ? "geometry" : "post-processed");

    SGCore::AttachmentReadback readback;
    if(!sourceFrameBuffer || !sourceFrameBuffer->readAttachmentPixels(attachment, readback))
    {
        SG_LOG_E("Smoke: current graphics API does not support frame readback.");
        m_exitCode = 2;
        SGCore::CoreMain::getWindow().setShouldClose(true);
        return;
    }

    RGBA8Image frame;
    if(!toRGBA8(readback, frame))
    {
        SG_LOG_E("Smoke: attachment layout (format {}, data type {}) can not be converted to RGBA8 for PNG.",
                 std::to_underlying(readback.m_format), std::to_underlying(readback.m_dataType));
        m_exitCode = 2;
        SGCore::CoreMain::getWindow().setShouldClose(true);
        return;
    }

    if(isOpenGLAPI(SGCore::CoreMain::getRenderer()->getGAPIType()))
    {
        flipVertically(frame);
    }

    const auto gapiName = std::string(gapiTypeToString(SGCore::CoreMain::getRenderer()->getGAPIType()));
    const auto outputPath = m_options.m_outputPath.empty()
                            ? std::filesystem::path("smoke_" + gapiName + ".png")
                            : m_options.m_outputPath;

    if(!writePNG(outputPath, frame))
    {
        SG_LOG_E("Smoke: failed to write '{}'.", outputPath.string());
        m_exitCode = 2;
    }
    else
    {
        SG_LOG_I("Smoke: frame {} captured to '{}' ({}x{}, {}).",
                 m_frameIndex, outputPath.string(), frame.m_width, frame.m_height, gapiName);
    }

    if(m_options.m_referencePath && m_exitCode == 0)
    {
        RGBA8Image reference;
        if(!readPNG(*m_options.m_referencePath, reference))
        {
            SG_LOG_E("Smoke: failed to read reference '{}'.", m_options.m_referencePath->string());
            m_exitCode = 2;
        }
        else
        {
            const auto result = compare(frame, reference, m_options.m_channelThreshold);

            if(result.m_sizeMismatch)
            {
                SG_LOG_E("Smoke: FAIL — size mismatch: got {}x{}, reference {}x{}.",
                         frame.m_width, frame.m_height, reference.m_width, reference.m_height);
                m_exitCode = 1;
            }
            else if(result.m_differingPixelsFraction > m_options.m_maxDifferingPixelsFraction)
            {
                const auto diffPath = outputPath.parent_path() / (outputPath.stem().string() + "_diff.png");
                std::ignore = writeDiffPNG(diffPath, frame, reference);

                SG_LOG_E("Smoke: FAIL — {:.3f}% of pixels differ (limit {:.3f}%), max channel diff {}. Diff: '{}'.",
                         result.m_differingPixelsFraction * 100.0,
                         m_options.m_maxDifferingPixelsFraction * 100.0,
                         result.m_maxChannelDifference, diffPath.string());
                m_exitCode = 1;
            }
            else
            {
                SG_LOG_I("Smoke: PASS — {:.3f}% of pixels differ, max channel diff {}.",
                         result.m_differingPixelsFraction * 100.0, result.m_maxChannelDifference);
            }
        }
    }

    SGCore::CoreMain::getWindow().setShouldClose(true);
}
