//
// Created by stuka on 26.01.2026.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct SGCORE_EXPORT EnableVolumetricPass : ECS::Component<EnableVolumetricPass, const EnableVolumetricPass>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::EnableVolumetricPass);

    private:
        bool m_dummy = false;
    };
}