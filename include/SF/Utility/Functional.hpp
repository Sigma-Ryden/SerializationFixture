#ifndef SF_UTILITY_FUNCTIONAL_HPP
#define SF_UTILITY_FUNCTIONAL_HPP

#include <cstddef> // size_t

#include <memory> // addressof
#include <valarray> // valarray

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace utility
{

template <class Container>
inline const typename Container::value_type* data(const Container& c) noexcept
{
    // if begin return iterator - we should deref it and take address again
    return std::addressof(*std::begin(c));
}

template <typename T>
inline const T* data(const std::valarray<T>& c) noexcept
{
    return std::begin(c);
}

template <class T, std::size_t N>
inline const T* data(const T (&array)[N]) noexcept
{
    return array;
}

template <class Container>
inline auto size(const Container& c) noexcept -> decltype(c.size())
{
    return c.size();
}

template <class T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept
{
    return N;
}

template <typename CharType,
          SF_REQUIRE(meta::is_character<CharType>::value)>
std::size_t size(const CharType* str) noexcept
{
    std::size_t count{};
    while (*str++ != CharType{}) ++count;

    return count;
}

} // namespace utility

} // namespace sf

#endif // SF_UTILITY_FUNCTIONAL_HPP
