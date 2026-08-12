#pragma once

/*
* MisterElect (MisterChoose)
* 02.10.2024
*/

#define move_operator(T) T& operator=(T&& other) noexcept
#define move_constructor(T) T(T&& other) noexcept
#define copy_operator(T) T& operator=(const T& other) noexcept
#define copy_constructor(T) T(const T& other) noexcept

#define SG_INSTANCEOF(data, type) SGCore::Utils::instanceof<type>(data)

#define SG_MAY_NORETURN __declspec(noreturn)

#define SG_CTOR(cls) cls() = default;
#define SG_COPY_CTOR(cls) cls(const cls&) = default;
#define SG_MOVE_CTOR(cls) cls(cls&& cls) noexcept = default;

#define SG_NO_CTOR(cls) cls() = delete;
#define SG_NO_COPY(cls) cls(const cls&) = delete;
#define SG_NO_MOVE(cls) cls(cls&& cls) noexcept = delete;

#define SG_CURRENT_LOCATION_STR SGCore::Utils::sourceLocationToString(std::source_location::current())

#define SG_STRINGIFY_MACRO(n) SG_STRINGIFY(n)
#define SG_STRINGIFY(n) #n

#ifdef _MSC_VER
#define SG_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define SG_NOINLINE __attribute__((noinline))
#endif

#ifdef _MSC_VER
#define SG_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define SG_FORCEINLINE __attribute__((always_inline))
#endif

#ifdef _MSC_VER
#define SG_CDECL __cdecl
#elif defined(__GNUC__) || defined(__clang__)
#define SG_CDECL __attribute__((cdecl))
#endif

#define SG_NOMANGLING extern "C"

#ifdef _MSC_VER
#define SG_DLEXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define SG_DLEXPORT __attribute__((visibility("default")))
#endif

#ifdef _MSC_VER
#define SG_DLIMPORT __declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
#define SG_DLIMPORT
#endif


#define SG_ARG_COUNT(...) \
    SG_ARG_COUNT_IMPL(0, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define SG_ARG_COUNT_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N

// Force expansion before token-paste / dispatch
#define SG_EXPAND(...) __VA_ARGS__
#define SG_CONCAT_PRIMITIVE(a, b) a##b
#define SG_CONCAT_DEFER(a, b) SG_CONCAT_PRIMITIVE(a, b)

// token-paste: SG_CONCANT(Foo, Bar, Baz) -> FooBarBaz
#define SG_CONCAT_IMPL_1(a) a
#define SG_CONCAT_IMPL_2(a, b) a##b
#define SG_CONCAT_IMPL_3(a, b, c) a##b##c
#define SG_CONCAT_IMPL_4(a, b, c, d) a##b##c##d
#define SG_CONCAT_IMPL_5(a, b, c, d, e) a##b##c##d##e
#define SG_CONCAT_IMPL_6(a, b, c, d, e, f) a##b##c##d##e##f
#define SG_CONCAT_IMPL_7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define SG_CONCAT_IMPL_8(a, b, c, d, e, f, g, h) a##b##c##d##e##f##g##h
#define SG_CONCAT_IMPL_9(a, b, c, d, e, f, g, h, i) a##b##c##d##e##f##g##h##i

#define SG_CONCAT_CHOOSER(_n) SG_CONCAT_DEFER(SG_CONCAT_IMPL_, _n)
#define SG_CONCAT(...) SG_EXPAND(SG_CONCAT_CHOOSER(SG_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__))

// insert separator between args: SG_CONCANT_SEP(_, A, B, C) -> A _ B _ C
#define SG_CONCAT_SEP_IMPL_1(sep, a) a
#define SG_CONCAT_SEP_IMPL_2(sep, a, b) a sep b
#define SG_CONCAT_SEP_IMPL_3(sep, a, b, c) a sep b sep c
#define SG_CONCAT_SEP_IMPL_4(sep, a, b, c, d) a sep b sep c sep d
#define SG_CONCAT_SEP_IMPL_5(sep, a, b, c, d, e) a sep b sep c sep d sep e
#define SG_CONCAT_SEP_IMPL_6(sep, a, b, c, d, e, f) a sep b sep c sep d sep e sep f
#define SG_CONCAT_SEP_IMPL_7(sep, a, b, c, d, e, f, g) a sep b sep c sep d sep e sep f sep g
#define SG_CONCAT_SEP_IMPL_8(sep, a, b, c, d, e, f, g, h) a sep b sep c sep d sep e sep f sep g sep h
#define SG_CONCAT_SEP_IMPL_9(sep, a, b, c, d, e, f, g, h, i) a sep b sep c sep d sep e sep f sep g sep h sep i

#define SG_CONCAT_SEP_CHOOSER(_n) SG_CONCAT_DEFER(SG_CONCAT_SEP_IMPL_, _n)
#define SG_CONCAT_SEP(sep, ...) SG_EXPAND(SG_CONCAT_SEP_CHOOSER(SG_ARG_COUNT(__VA_ARGS__))(sep, __VA_ARGS__))

// comma-separated: SG_CONCAT_COMMA(A, B, C) -> A, B, C
#define SG_CONCAT_COMMA_IMPL_1(a) a
#define SG_CONCAT_COMMA_IMPL_2(a, b) a, b
#define SG_CONCAT_COMMA_IMPL_3(a, b, c) a, b, c
#define SG_CONCAT_COMMA_IMPL_4(a, b, c, d) a, b, c, d
#define SG_CONCAT_COMMA_IMPL_5(a, b, c, d, e) a, b, c, d, e
#define SG_CONCAT_COMMA_IMPL_6(a, b, c, d, e, f) a, b, c, d, e, f
#define SG_CONCAT_COMMA_IMPL_7(a, b, c, d, e, f, g) a, b, c, d, e, f, g
#define SG_CONCAT_COMMA_IMPL_8(a, b, c, d, e, f, g, h) a, b, c, d, e, f, g, h
#define SG_CONCAT_COMMA_IMPL_9(a, b, c, d, e, f, g, h, i) a, b, c, d, e, f, g, h, i

#define SG_CONCAT_COMMA_CHOOSER(_n) SG_CONCAT_DEFER(SG_CONCAT_COMMA_IMPL_, _n)
#define SG_CONCAT_COMMA(...) SG_EXPAND(SG_CONCAT_COMMA_CHOOSER(SG_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__))