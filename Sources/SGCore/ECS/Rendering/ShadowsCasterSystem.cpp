//
// Created by stuka on 27.07.2023.
//

#include "ShadowsCasterSystem.h"
#include "ShadowsCasterComponent.h"
#include "SGCore/ECS/Transformations/TransformComponent.h"
#include "MeshComponent.h"

void Core::ECS::ShadowsCasterSystem::update(const std::shared_ptr<Scene>& scene)
{
    size_t totalShadowCasters = 0;

    for(const auto& shadowsCasterEntity : scene->m_entities)
    {
        std::shared_ptr<ShadowsCasterComponent> shadowsCasterComponent =
                shadowsCasterEntity->getComponent<ShadowsCasterComponent>();

        if(shadowsCasterComponent)
        {
            shadowsCasterComponent->m_frameBuffer->bind();

            for(const auto& entity : scene->m_entities)
            {
                std::shared_ptr<MeshComponent> meshComponent = entity->getComponent<MeshComponent>();

                if(meshComponent)
                {
                    meshComponent->m_mesh->m_material->m_shader->useTexture(
                            "shadowMaps[" + std::to_string(totalShadowCasters) + "]",
                            0);
                }
            }

            shadowsCasterComponent->m_frameBuffer->unbind();

            totalShadowCasters++;
        }
    }
}

void Core::ECS::ShadowsCasterSystem::update(const std::shared_ptr<Scene>& scene,
                                            const std::shared_ptr<Core::ECS::Entity>& entity)
{
    std::shared_ptr<ShadowsCasterComponent> shadowsCasterComponent = entity->getComponent<ShadowsCasterComponent>();

    if(!shadowsCasterComponent) return;

    for(auto& sceneEntity : scene->m_entities)
    {
        std::shared_ptr<TransformComponent> transformComponent = sceneEntity->getComponent<TransformComponent>();
        std::shared_ptr<MeshComponent> meshComponent = sceneEntity->getComponent<MeshComponent>();

        if(!transformComponent || !meshComponent) continue;

        // TODO: make rendering
        Core::Main::CoreMain::getRenderer().renderMesh(shadowsCasterComponent, transformComponent, meshComponent);
    }
}

void Core::ECS::ShadowsCasterSystem::deltaUpdate(const std::shared_ptr<Scene>& scene,
                                                 const std::shared_ptr<Core::ECS::Entity>& entity,
                                                 const double& deltaTime)
{

}
