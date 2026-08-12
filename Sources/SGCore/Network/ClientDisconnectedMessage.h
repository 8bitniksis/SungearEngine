//
// Created by stuka on 19.08.2025.
//

#pragma once

#include "sgcore_export.h"
#include "SGCore/Utils/StaticTypeID.h"

namespace SGCore::Net
{
    struct SGCORE_EXPORT ClientDisconnectedMessage
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::Net::ClientDisconnectedMessage);

        static constexpr bool use_rudp = true;
    };
}