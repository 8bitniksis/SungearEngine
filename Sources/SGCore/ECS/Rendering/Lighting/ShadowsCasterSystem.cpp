//
// Created by stuka on 27.07.2023.
//

#include "ShadowsCasterSystem.h"
#include "ShadowsCasterComponent.h"
#include "SGCore/ECS/Transformations/TransformComponent.h"
#include "SGCore/ECS/Rendering/MeshComponent.h"
#include "SGCore/ECS/Rendering/SkyboxComponent.h"
#include "SGCore/ECS/ECSWorld.h"
#include "SGCore/ECS/Rendering/MeshedEntitiesCollectorSystem.h"

void Core::ECS::ShadowsCasterSystem::FPSRelativeFixedUpdate(const std::shared_ptr<Scene>& scene)
{
    double t0 = glfwGetTime();

    auto thisSystemCachedEntities = ECSWorld::getSystemCachedEntities<ShadowsCasterSystem>();
    auto meshedCachedEntities = ECSWorld::getSystemCachedEntities<MeshedEntitiesCollectorSystem>();

    if(thisSystemCachedEntities == nullptr || meshedCachedEntities == nullptr) return;

    size_t totalShadowCasters = 0;

    for (const auto& cachedEntities : thisSystemCachedEntities->m_cachedEntities)
    {
        if(cachedEntities.second == nullptr) continue;

        // todo: make process all ShadowsCasterComponent (cachedEntities.second->getComponents)
        std::shared_ptr<ShadowsCasterComponent> shadowsCasterComponent = cachedEntities.second->getComponent<ShadowsCasterComponent>();
        std::shared_ptr<TransformComponent> shadowCasterTransform = cachedEntities.second->getComponent<TransformComponent>();

        if (!shadowCasterTransform || !shadowsCasterComponent) continue;

        shadowsCasterComponent->m_frameBuffer->bind()->clear();
        Core::Main::CoreMain::getRenderer().prepareUniformBuffers(shadowsCasterComponent, nullptr);

        for(const auto& meshedEntity: meshedCachedEntities->m_cachedEntities)
        {
            if(meshedEntity.second == nullptr) continue;

            // todo: make caching for entities with TransformComponent and MeshComponent
            auto entityTransformComponent = meshedEntity.second->getComponent<TransformComponent>();

            auto meshComponents =
                    meshedEntity.second->getComponents<MeshComponent>();

            if(!entityTransformComponent || meshComponents.empty()) continue;

            for(const auto& meshComponent : meshComponents)
            {
                const auto& materialShader = meshComponent->m_mesh->m_material->getCurrentShader();

                if(!materialShader) return;

                std::uint8_t shadowMapsBlockOffset = meshComponent->
                        m_mesh->
                        m_material->getBlocks()[SGMaterialTextureType::SGTP_SHADOW_MAP].m_texturesUnitOffset;

                std::uint8_t finalTextureBlock = shadowMapsBlockOffset + totalShadowCasters;

                shadowsCasterComponent->m_frameBuffer->bindAttachment(
                        SG_FRAMEBUFFER_DEPTH_ATTACHMENT_NAME,
                        finalTextureBlock
                );

                std::string totalShadowsCastersStr = std::to_string(totalShadowCasters);

                materialShader->bind();

                materialShader->useTexture(
                        "shadowsCastersShadowMaps[" + totalShadowsCastersStr + "]",
                        finalTextureBlock
                );

                // todo: maybe make uniform buffer for shadows casters
                materialShader->useMatrix(
                        "shadowsCasters[" + totalShadowsCastersStr + "].shadowsCasterSpace",
                        shadowsCasterComponent->m_projectionMatrix * shadowsCasterComponent->m_viewMatrix
                );

                materialShader->useVectorf(
                        "shadowsCasters[" + totalShadowsCastersStr + "].position",
                        shadowCasterTransform->m_position
                );

                //auto lastFacesCullingType = meshComponent->m_facesCullingFaceType;
                //auto lastFacesCullingPolygonsOrder = meshComponent->m_facesCullingPolygonsOrder;
                meshComponent->m_mesh->m_material->setCurrentShader(SGMAT_SHADOW_GEN_SHADER_NAME);
                //meshComponent->m_facesCullingFaceType = SGFaceType::SGG_FRONT_FACE;
                //meshComponent->m_facesCullingPolygonsOrder = SGPolygonsOrder::SGG_CCW;

                Core::Main::CoreMain::getRenderer().renderMesh(entityTransformComponent, meshComponent);

                meshComponent->m_mesh->m_material->setCurrentShader(SGMAT_STANDARD_SHADER_NAME);
                //meshComponent->m_facesCullingFaceType = lastFacesCullingType;
                //meshComponent->m_facesCullingPolygonsOrder = lastFacesCullingPolygonsOrder;
            }
        }

        shadowsCasterComponent->m_frameBuffer->bind()->unbind();

        totalShadowCasters++;
    }

    double t1 = glfwGetTime();

    // 13,601600
    // std::cout << "ms for shadows caster: " << std::to_string((t1 - t0) * 1000.0) << std::endl;
}

void Core::ECS::ShadowsCasterSystem::cacheEntity(const std::shared_ptr<Core::ECS::Entity>& entity) const
{
    ECSWorld::cacheComponents<ShadowsCasterSystem, ShadowsCasterComponent, TransformComponent>(entity);
}

