//
// Created by ilya on 10.03.24.
//

#pragma once

#include <unordered_map>

#include "SGCore/Main/CoreGlobals.h"
#include "SGCore/ECS/Component.h"

namespace SGCore
{
    struct OctreeNode;
    
    struct SGCORE_EXPORT OctreeCullable : ECS::Component<OctreeCullable, const OctreeCullable>
    {
        SG_IMPLEMENT_STATIC_TYPE_ID(SGCore::OctreeCullable);

        std::unordered_map<ECS::entity_t, Weak<OctreeNode>> m_parentNodes;
        
    private:
        bool m_dummy = true;
    };
}
