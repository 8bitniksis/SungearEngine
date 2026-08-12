//
// Created by ilya on 10.03.24.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct SGCORE_EXPORT IgnoreOctrees : ECS::Component<IgnoreOctrees, const IgnoreOctrees>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::IgnoreOctrees);

    private:
        bool m_dummy = true;
    };
}
