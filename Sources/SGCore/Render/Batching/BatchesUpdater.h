//
// Created by stuka on 18.07.2025.
//

#pragma once

#include "SGCore/Scene/ISystem.h"

namespace SGCore
{
    struct BatchesUpdater : ISystem
    {
        SG_IMPLEMENT_TYPE_ID(SGCore::BatchesUpdater)

        void update(double dt, double fixedDt) noexcept final;
    };
}
