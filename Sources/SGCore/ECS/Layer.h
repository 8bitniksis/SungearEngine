//
// Created by stuka on 24.10.2023.
//

#ifndef SUNGEARENGINE_LAYER_H
#define SUNGEARENGINE_LAYER_H

#include <list>
#include "Entity.h"

#define SG_LAYER_OPAQUE_NAME        "Opaque"
#define SG_LAYER_TRANSPARENT_NAME   "Transparent"

namespace Core::ECS
{
    class Layer
    {
        friend class Scene;
        friend class ISystem;
        friend class LayersComparator;

    public:
        std::list<std::shared_ptr<Entity>> m_entities;

        std::string m_name;

    private:
        size_t m_index;
    };

    struct LayersComparator
    {
        bool operator()(const std::shared_ptr<Layer>& l0, const std::shared_ptr<Layer>& l1) const
        {
            return l0->m_index > l1->m_index;
        }
    };
}

#endif //SUNGEARENGINE_LAYER_H
