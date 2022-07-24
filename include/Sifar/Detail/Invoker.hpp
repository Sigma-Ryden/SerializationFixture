#ifndef SIFAR_DETAIL_INVOKER_HPP
#define SIFAR_DETAIL_INVOKER_HPP

#include <cstddef> // size_t

#include <Sifar/Detail/Meta.hpp>

#define SIFAR_POW2(n) (std::size_t(1) << (n))

#define SIFAR_REQUIRE(condition)                                                                        \
    ::sifar::meta::require<(bool)(condition)> = 0

#define SIFAR_INVOKE(ConstKey, RuntimeKey, Offset, Function)                                            \
    if (RuntimeKey - ConstKey >= Offset) {                                                              \
        constexpr auto INVOKE_NEXT = ConstKey + Offset;                                                 \
        return Function;                                                                                \
    }

#define SIFAR_INVOKE_0(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(0), F)

#define SIFAR_INVOKE_1(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(1), F) SIFAR_INVOKE_0(C, V, F)
#define SIFAR_INVOKE_2(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(2), F) SIFAR_INVOKE_1(C, V, F)
#define SIFAR_INVOKE_3(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(3), F) SIFAR_INVOKE_2(C, V, F)
#define SIFAR_INVOKE_4(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(4), F) SIFAR_INVOKE_3(C, V, F)

#define SIFAR_INVOKE_5(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(5), F) SIFAR_INVOKE_4(C, V, F)
#define SIFAR_INVOKE_6(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(6), F) SIFAR_INVOKE_5(C, V, F)
#define SIFAR_INVOKE_7(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(7), F) SIFAR_INVOKE_6(C, V, F)
#define SIFAR_INVOKE_8(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(8), F) SIFAR_INVOKE_7(C, V, F)

#define SIFAR_INVOKE_9(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(9), F) SIFAR_INVOKE_8(C, V, F)
#define SIFAR_INVOKE_10(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(10), F) SIFAR_INVOKE_9(C, V, F)
#define SIFAR_INVOKE_11(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(11), F) SIFAR_INVOKE_10(C, V, F)
#define SIFAR_INVOKE_12(C, V, F) SIFAR_INVOKE(C, V, SIFAR_POW2(12), F) SIFAR_INVOKE_11(C, V, F)

#define SIFAR_INVOKE_N(N, C, V, F) SIFAR_INVOKE_##N(C, V, F)

#if (true) || defined(SIFAR_INVOKE_DEFAULT) // 128
#   define SIFAR_INVOKE_DEPTH() SIFAR_POW2(7)
#   define SIFAR_INVOKE_BODY(ConstKey, RuntimeKey, Function)                                            \
        SIFAR_INVOKE_N(7, ConstKey, RuntimeKey, Function)
#endif

#ifdef SIFAR_INVOKE_LOW // 256
#   undef SIFAR_INVOKE_DEPTH
#   undef SIFAR_INVOKE_BODY
#   define SIFAR_INVOKE_DEPTH() SIFAR_POW2(8)
#   define SIFAR_INVOKE_BODY(ConstKey, RuntimeKey, Function)                                            \
        SIFAR_INVOKE_N(8, ConstKey, RuntimeKey, Function)
#endif // SIFAR_INVOKE_MEDIUM

#ifdef SIFAR_INVOKE_MEDIUM // 512
#   undef SIFAR_INVOKE_DEPTH
#   undef SIFAR_INVOKE_BODY
#   define SIFAR_INVOKE_DEPTH() SIFAR_POW2(9)
#   define SIFAR_INVOKE_BODY(ConstKey, RuntimeKey, Function)                                            \
        SIFAR_INVOKE_N(9, ConstKey, RuntimeKey, Function)
#endif // SIFAR_INVOKE_MEDIUM

#ifdef SIFAR_INVOKE_HIGH // 1024
#   undef SIFAR_INVOKE_DEPTH
#   undef SIFAR_INVOKE_BODY
#   define SIFAR_INVOKE_DEPTH() SIFAR_POW2(10)
#   define SIFAR_INVOKE_BODY(ConstKey, RuntimeKey, Function)                                            \
        SIFAR_INVOKE_N(10, ConstKey, RuntimeKey, Function)
#endif // SIFAR_INVOKE_HIGH

#ifdef INVOKE_HIGHEST // 2048
#   undef SIFAR_INVOKE_DEPTH
#   undef SIFAR_INVOKE_BODY
#   define SIFAR_INVOKE_DEPTH() SIFAR_POW2(11)
#   define SIFAR_INVOKE_BODY(ConstKey, RuntimeKey, Function)                                            \
        SIFAR_INVOKE_N(11, ConstKey, RuntimeKey, Function)
#endif // INVOKE_HIGHEST

#define SIFAR_INVOKE_END(ConstKey)                                                                      \
    std::size_t ConstKey, SIFAR_REQUIRE(ConstKey >= SIFAR_INVOKE_DEPTH())

#define SIFAR_INVOKE_BEGIN(ConstKey)                                                                    \
    std::size_t ConstKey = 0, SIFAR_REQUIRE(ConstKey < SIFAR_INVOKE_DEPTH())

#endif // SIFAR_DETAIL_INVOKER_HPP

