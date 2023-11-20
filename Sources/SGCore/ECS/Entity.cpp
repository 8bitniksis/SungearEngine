//
// Created by stuka on 02.05.2023.
//

#include "Entity.h"

#include "ECSWorld.h"

#include "SGCore/ECS/Rendering/Lighting/ShadowsCaster.h"
#include "SGCore/ECS/Rendering/Lighting/DirectionalLight.h"

void SGCore::Entity::addComponent(const std::shared_ptr<IComponent>& component) noexcept
{
    m_components.push_back(component);

    // recache all systems, that can use this entity
    ECSWorld::recacheEntity(shared_from_this());
}

std::shared_ptr<SGCore::Layer> SGCore::Entity::getLayer() const noexcept
{
    return m_layer.lock();
}
