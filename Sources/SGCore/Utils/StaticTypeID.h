//
// Created by stuka on 23.10.2024.
//

#pragma once

#include <set>
#include <iostream>
#include <fmt/format.h>
#include <sgcore_export.h>

#include "SGCore/Utils/TypeTraits.h"

#ifdef SUNGEAR_DEBUG
#include "SGCore/Logger/Logger.h"
#endif

/// Pass current class type (variadic — commas in templates are OK). PLEASE, QUALIFY YOUR TYPE BY NAMESPACE.\n
/// Use this macro in derived types to implement function \p getTypeID() that overrides virtual function in base type\n
/// \p getTypeID() is needed to get real static type ID of object.\n
/// Implementation of this macro must has public access.
#define SG_IMPLEMENT_TYPE_ID(...)                          \
static std::uint64_t getTypeIDStatic() { static std::uint64_t typeID = SGCore::StaticTypeID<__VA_ARGS__>::setID(SGCore::constexprHash(#__VA_ARGS__)); return typeID; }   \
SG_OVERRIDE_TYPE_ID(__VA_ARGS__)

/// Pass current class type (variadic — commas in templates are OK). PLEASE, QUALIFY YOUR TYPE BY NAMESPACE. \n
/// Use this macro in base types to implement virtual function \p getTypeID() .\n
/// \p getTypeID() is needed to get real static type ID of object.\n
/// Implementation of this macro must has public access.
#define SG_IMPLEMENT_TYPE_ID_BASE(...)                          \
static std::uint64_t getTypeIDStatic() { static std::uint64_t typeID = SGCore::StaticTypeID<__VA_ARGS__>::setID(SGCore::constexprHash(#__VA_ARGS__)); return typeID; }        \
virtual std::uint64_t getTypeID() const noexcept { return __VA_ARGS__::getTypeIDStatic(); }

/// Pass current class type (variadic — commas in templates are OK). PLEASE, QUALIFY YOUR TYPE BY NAMESPACE.\n
/// Creates only static function to get type ID without virtual function to get type ID of object.
/// Implementation of this macro must has public access.
#define SG_IMPLEMENT_STATIC_TYPE_ID(...)                    \
static std::uint64_t getTypeIDStatic() { static std::uint64_t typeID = SGCore::StaticTypeID<__VA_ARGS__>::setID(SGCore::constexprHash(#__VA_ARGS__)); return typeID; }

/// Pass current class type (variadic — commas in templates are OK). PLEASE, QUALIFY YOUR TYPE BY NAMESPACE.\n
/// Overrides only base virtual function to get type ID.
/// Implementation of this macro must has public access.
#define SG_OVERRIDE_TYPE_ID(...)                    \
std::uint64_t getTypeID() const noexcept final { return __VA_ARGS__::getTypeIDStatic(); }


namespace SGCore
{
    struct SGCORE_EXPORT StaticTypeIDsContainer
    {
        template<typename>
        friend struct StaticTypeID;

        static bool isTypeIDExists(std::uint64_t typeID) noexcept;

    private:
        static void addTypeID(std::uint64_t typeID) noexcept;

        static std::set<std::uint64_t>& getExistingTypeIDs() noexcept;
    };

    template<typename T>
    struct StaticTypeID
    {
        friend T;

        static std::uint64_t getID()
        {
            return m_typeID;
        }

    private:
        static std::uint64_t setID(std::uint64_t typeID)
        {
            m_typeID = typeID;
            if(StaticTypeIDsContainer::isTypeIDExists(typeID))
            {
                const std::string message = fmt::format("Can not set type ID '{}' for type '{}': some other type with this ID is already exists.", typeID, typeid(T).name());
                std::cerr << message;
            }

#ifdef SUNGEAR_DEBUG
            SG_LOG_I("Set type ID {} for type '{}'", m_typeID, typeid(T).name());
#endif

            StaticTypeIDsContainer::addTypeID(m_typeID);

            return m_typeID;
        }

        static inline std::uint64_t m_typeID = 0;
    };
}
