//
// Created by stuka on 02.02.2024.
//

#pragma once

#include "SGCore/Scene/ISystem.h"

namespace SGCore
{
    class SGCORE_EXPORT BoxGizmosRenderer : public ISystem
    {
        SG_IMPLEMENT_TYPE_ID(SGCore::BoxGizmosRenderer)

        void fixedUpdate(double dt, double fixedDt) final;
    };
}
