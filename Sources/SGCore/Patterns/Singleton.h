//
// Created by stuka on 21.07.2023.
//

#ifndef SUNGEARENGINE_SINGLETON_H
#define SUNGEARENGINE_SINGLETON_H

#include <memory>

#define SG_FULL_SINGLETON(cls)                          \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls() = default;                                \
        cls(const cls&) = default;                      \
        cls(cls&&) noexcept = default;

#define SG_CTOR_SINGLETON(cls)                          \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls(const cls&) = default;

#define SG_COPY_SINGLETON(cls)                          \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls(const cls&) = default;

#define SG_MOVE_SINGLETON(cls)                          \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls(cls&&) noexcept = default;

#define SG_CUSTOM_CTOR_SINGLETON(cls)                   \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls();

#define SG_CUSTOM_COPY_SINGLETON(cls)                   \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls(const cls&);

#define SG_CUSTOM_MOVE_SINGLETON(cls)                   \
        friend struct ::SGSingleton;                    \
    protected:                                          \
        cls(cls&&) noexcept;


struct SGSingleton
{
    template<class T>
    static std::shared_ptr<T> getSharedPtrInstance()
    {
        static std::shared_ptr<T> s_instance(new T);
        return s_instance;
    }

    template<class T>
    static T& getInstance()
    {
        static T s_instance;
        return s_instance;
    }
};

#endif //SUNGEARENGINE_SINGLETON_H
