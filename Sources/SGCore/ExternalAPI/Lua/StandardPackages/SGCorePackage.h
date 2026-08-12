//
// Created by stuka on 12.02.2026.
//

#pragma once

#include "SGCore/ExternalAPI/Lua/Package.h"

namespace SGCore::Lua
{
    struct SGCorePackage final : Package
    {
        sg_declare_lua_package(SGCore)

        SG_SERDE_AS_FRIEND()

        SG_IMPLEMENT_TYPE_ID(SGCore::Lua::SGCorePackage)

    private:
        void doLoadInState(sol::state& luaState, function_result& packageResult) noexcept override;
    };
}
