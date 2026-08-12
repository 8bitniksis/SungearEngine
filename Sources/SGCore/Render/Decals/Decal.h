//
// Created by stuka on 27.04.2025.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct SGCORE_EXPORT Decal : ECS::Component<Decal, const Decal>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::Decal);

    private:
        bool m_dummy = false;
    };
}
