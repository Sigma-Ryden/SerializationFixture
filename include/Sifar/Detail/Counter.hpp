#ifndef SIFAR_DETAIL_COUNTER_HPP
#define SIFAR_DETAIL_COUNTER_HPP

#include <cstddef> // size_t

#include <Sifar/Detail/Meta.hpp>

#ifdef SIFAR_EXPERIMENTAL
#define COUNTER_DETAIL(R, I)                                                                            \
    ::sifar::detail::constexpr_counter(                                                                 \
        ::sifar::meta::constant_index<R>(),                                                             \
        ::sifar::meta::constant_index<I>())

#define COUNTER_VALUE()                                                                                 \
    COUNTER_DETAIL(1, COUNTER_DETAIL(2,                                                                 \
    COUNTER_DETAIL(4, COUNTER_DETAIL(8,                                                                 \
    COUNTER_DETAIL(16, COUNTER_DETAIL(32,                                                               \
    COUNTER_DETAIL(64, COUNTER_DETAIL(128,                                                              \
    COUNTER_DETAIL(256, COUNTER_DETAIL(512, 0))))))))))

#define COUNTER_INCREMENT()                                                                             \
    namespace sifar { namespace detail {                                                                \
    constexpr ::sifar::meta::constant_index<COUNTER_VALUE() + 1> constexpr_counter(                     \
        ::sifar::meta::constant_index<(COUNTER_VALUE() + 1) & ~COUNTER_VALUE()>,                        \
        ::sifar::meta::constant_index<(COUNTER_VALUE() + 1) & COUNTER_VALUE()>)                         \
        { return {}; }                                                                                  \
    }}

#define COUNTER_DEFINE()                                                                                \
    namespace sifar { namespace detail {                                                                \
    template <std::size_t R, std::size_t I>                                                             \
    constexpr ::sifar::meta::constant_index<I> constexpr_counter(                                       \
        ::sifar::meta::constant_index<R>,                                                               \
        ::sifar::meta::constant_index<I>) { return {}; }                                                \
    }}

COUNTER_DEFINE()
#endif // SIFAR_EXPERIMENTAL

#endif // SIFAR_DETAIL_COUNTER_HPP
