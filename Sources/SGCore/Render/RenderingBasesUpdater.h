//
// Created by stuka on 03.02.2024.
//

#pragma once

#include "SGCore/Scene/ISystem.h"

namespace SGCore
{
    struct SGCORE_EXPORT RenderingBasesUpdater : public ISystem
    {
        SG_IMPLEMENT_TYPE_ID(SGCore::RenderingBasesUpdater)

        void update(double dt, double fixedDt) final;
    };
}
