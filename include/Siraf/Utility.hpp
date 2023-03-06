#ifndef SIRAF_UTILITY_HPP
#define SIRAF_UTILITY_HPP

#include <cstddef> // size_t

#include <memory> // addressof
#include <valarray> // valarray

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace utility
{

template <class Container>
inline const typename Container::value_type* data(const Container& c)
{
    // if begin return iterator - we should deref it and take address again
    return std::addressof(*std::begin(c));
}

template <typename T>
inline const T* data(const std::valarray<T>& c)
{
    return std::begin(c);
}

template <class T, std::size_t N>
inline const T* data(const T (&array)[N]) noexcept
{
    return array;
}

template <class Container>
inline auto size(const Container& c) -> decltype(c.size())
{
    return c.size();
}

template <class T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept
{
    return N;
}

template <typename CharType,
          SIREQUIRE(meta::is_character<CharType>())>
std::size_t size(const CharType* str) noexcept
{
    std::size_t count = 0;
    while (*str++ != CharType(0)) ++count;

    return count;
}

} // namespace utility

} // namespace siraf

#endif // SIRAF_UTILITY_HPP
