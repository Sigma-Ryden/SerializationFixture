#ifndef SF_UTILITY_EXPANDING_HPP
#define SF_UTILITY_EXPANDING_HPP

#if __cplusplus >= 201703L

#include <SF/Detail/Meta.hpp>

namespace sf
{

namespace detail
{

template <std::size_t I> using dispatch = std::integral_constant<std::size_t, I>;

// will hide into macro
template <class T> decltype(auto) expand(T& object, dispatch<0>)
{
    return std::forward_as_tuple();
}

template <class T> decltype(auto) expand(T& object, dispatch<1>)
{
    auto& [_1] = object;
    return std::forward_as_tuple(_1);
}

template <class T> decltype(auto) expand(T& object, dispatch<2>)
{
    auto& [_1, _2] = object;
    return std::forward_as_tuple(_1, _2);
}

template <class T> decltype(auto) expand(T& object, dispatch<3>)
{
    auto& [_1, _2, _3] = object;
    return std::forward_as_tuple(_1, _2, _3);
}
// ~ and etc.

} // namespace detail

namespace utility
{

template <class T> decltype(auto) expand(T& object)
{
    constexpr auto size = meta::aggregate_size<T>();
    return detail::expand(object, detail::dispatch<size>{});
}

} // namespace utility

} // namespace sf

#endif // if

#endif // SF_UTILITY_EXPANDING_HPP
