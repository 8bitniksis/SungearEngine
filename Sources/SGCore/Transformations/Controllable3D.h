//
// Created by stuka on 03.02.2024.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct SGCORE_EXPORT Controllable3D : ECS::Component<Controllable3D, const Controllable3D>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::Controllable3D);

        float m_movementSpeed = 0.075f;
        float m_rotationSensitive = 0.2f;
        bool m_inverseMouse = false;

        glm::vec3 m_pitchYawRoll {};
    };
}
