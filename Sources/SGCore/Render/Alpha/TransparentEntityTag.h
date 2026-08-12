//
// Created by stuka on 31.12.2024.
//

#pragma once

#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct TransparentEntityTag : public ECS::Component<TransparentEntityTag, const TransparentEntityTag>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::TransparentEntityTag);

    private:
        bool m_dummy = false;
    };
}
