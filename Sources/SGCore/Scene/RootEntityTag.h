//
// Created by stuka on 17.03.2026.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct SGCORE_EXPORT RootEntityTag : ECS::Component<RootEntityTag, const RootEntityTag>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::RootEntityTag);
    };
}