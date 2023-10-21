//
// Created by stuka on 02.05.2023.
//

#include "Entity.h"

#include "ECSWorld.h"

#include "SGCore/ECS/Rendering/Lighting/ShadowsCasterComponent.h"
#include "SGCore/ECS/Rendering/Lighting/DirectionalLightComponent.h"

void Core::ECS::Entity::addComponent(const std::shared_ptr<IComponent>& component) noexcept
{
    m_components.push_back(component);

    if(SG_INSTANCEOF(component.get(), ShadowsCasterComponent))
    {
        if(Scene::getCurrentScene())
        {
            // todo: do for scene where entity is placed
            Scene::getCurrentScene()->setShadowsCastersNum(Scene::getCurrentScene()->getShadowsCastersNum() + 1);
        }
    }

    if(SG_INSTANCEOF(component.get(), DirectionalLightComponent))
    {
        if(Scene::getCurrentScene())
        {
            // todo: do for scene where entity is placed
            Scene::getCurrentScene()->setDirectionalLightsNum(
                    Scene::getCurrentScene()->getDirectionalLightsNum() + 1
                    );
        }
    }

    // recache all systems, that can use this entity
    ECSWorld::recacheEntity(shared_from_this());
}
