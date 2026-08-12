//
// Created by stuka on 21.07.2025.
//

#pragma once

#include "SGCore/Scene/ISystem.h"

namespace SGCore
{
    struct SGCORE_EXPORT InstancingUpdater : ISystem
    {
        SG_IMPLEMENT_TYPE_ID(SGCore::InstancingUpdater)

        void update(double dt, double fixedDt) noexcept final;
    };
}
