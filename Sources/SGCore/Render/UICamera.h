//
// Created by ilya on 25.02.24.
//

#pragma once

#include "sgcore_export.h"

namespace SGCore
{
    struct SGCORE_EXPORT UICamera final : ECS::Component<UICamera, const UICamera>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::UICamera);

    private:
        volatile int m_dummy = 0;
    };
}
