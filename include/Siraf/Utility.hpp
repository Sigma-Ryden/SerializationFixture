#ifndef SIRAF_UTILITY_HPP
#define SIRAF_UTILITY_HPP

#include <cstdint> // int32_t, uint32_t, int64_t, uint64_t

#include <memory> // addressof
#include <valarray> // valarray

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace let
{
using i8 = std::int8_t;
using u8 = std::uint8_t;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i64 = std::int64_t;
using u64 = std::uint64_t;

} // namespace let

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

// clean up
#undef _NULL_CHARACTER_FUNCTION_GENERIC

#endif // SIRAF_UTILITY_HPP
