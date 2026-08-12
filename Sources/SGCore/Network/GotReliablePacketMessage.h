//
// Created by stuka on 17.07.2026.
//

#pragma once

#include "sgcore_export.h"
#include "SGCore/Utils/StaticTypeID.h"

namespace SGCore::Net
{
    struct SGCORE_EXPORT GotReliablePacketMessage
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::Net::GotReliablePacketMessage);
    };
}