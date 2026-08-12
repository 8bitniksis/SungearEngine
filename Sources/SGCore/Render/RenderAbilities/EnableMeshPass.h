//
// Created by stuka on 26.01.2026.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct SGCORE_EXPORT EnableMeshPass : ECS::Component<EnableMeshPass, const EnableMeshPass>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::EnableMeshPass);

    private:
        bool m_dummy = false;
    };
}
