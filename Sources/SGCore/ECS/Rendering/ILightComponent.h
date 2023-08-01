//
// Created by stuka on 01.08.2023.
//

#ifndef SUNGEARENGINE_ILIGHTCOMPONENT_H
#define SUNGEARENGINE_ILIGHTCOMPONENT_H

#include "SGCore/ECS/IComponent.h"
#include <glm/glm.hpp>

namespace Core::ECS
{
    class ILightComponent : public IComponent
    {
    private:
        void init() noexcept override { }

    public:
        glm::vec4 m_color { 1.0, 1.0, 1.0, 1.0 };
    };
}

#endif //SUNGEARENGINE_ILIGHTCOMPONENT_H
