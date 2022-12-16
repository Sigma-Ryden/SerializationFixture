#ifndef SIRAF_UTILITY_HPP
#define SIRAF_UTILITY_HPP

#include <cstdint> // int32_t, uint32_t, int64_t, uint64_t

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define _NULL_CHARACTER_FUNCTION_GENERIC(char_type, char_value)                                         \
    template <typename T, SIREQUIRE(std::is_same<meta::remove_cv<T>, char_type>::value)>                \
    constexpr char_type null_character() noexcept { return char_value; }

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

template <typename T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept { return N; }

template <class InIt, class OutIt>
OutIt copy(InIt first, InIt last, OutIt dst_first) noexcept
{
    while (first != last)
    {
        *dst_first = *first;

        ++dst_first;
        ++first;
    }

    return dst_first;
}

_NULL_CHARACTER_FUNCTION_GENERIC(char, '\0')
_NULL_CHARACTER_FUNCTION_GENERIC(wchar_t, L'\0')
_NULL_CHARACTER_FUNCTION_GENERIC(char16_t, u'\0')
_NULL_CHARACTER_FUNCTION_GENERIC(char32_t, U'\0')

template <typename CharType, SIREQUIRE(meta::is_character<CharType>())>
constexpr std::size_t static_size(const CharType* str, std::size_t count = 0) noexcept
{
    return *str == null_character<CharType>() ? count : static_size(str + 1, count + 1);
}

template <typename CharType, SIREQUIRE(meta::is_character<CharType>())>
std::size_t size(const CharType* str) noexcept
{
    std::size_t count = 0;
    while (*str++ != null_character<CharType>()) ++count;

    return count;
}

} // namespace utility

} // namespace siraf

// clean up
#undef _NULL_CHARACTER_FUNCTION_GENERIC

#endif // SIRAF_UTILITY_HPP
