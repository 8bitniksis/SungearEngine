//
// Created by stuka on 16.05.2026.
//

#pragma once

#include "SGCore/Utils/StaticTypeID.h"
#include "Registry.h"

#define SG_IMPLEMENT_SINGLETON_COMPONENT(ComponentT) \
    SG_IMPLEMENT_STATIC_TYPE_ID(ComponentT);

namespace SGCore::ECS
{
    template<typename DerivedT>
    struct SingletonComponent
    {
        SingletonComponent() noexcept
        {
            static volatile bool staticInit = []() {
                registry_t::registerSingleton<DerivedT>();

                return true;
            }();
        }

        ~SingletonComponent() noexcept
        {
            static_assert(
                requires { DerivedT::getTypeIDStatic(); },
                "Please, implement singleton component SG_IMPLEMENT_SINGLETON_COMPONENT() macro."
            );
        }
    };
}