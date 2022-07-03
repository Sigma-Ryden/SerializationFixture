#ifndef SIFAR_DETAIL_TOOLS_HPP
#define SIFAR_DETAIL_TOOLS_HPP

#include <cstddef> // size_t

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace utility
{

template <typename T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept { return N; }

template <typename T>
constexpr const char* const_byte_cast(T& data) noexcept
{
    return reinterpret_cast<const char*>(&data);
}

template <typename T>
constexpr char* byte_cast(T& data) noexcept
{
    return reinterpret_cast<char*>(&data);
}

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

template <typename T, meta::require<std::is_same<meta::remove_cv<T>, char>::value> = 0>
inline constexpr char null_character() noexcept { return '\0'; }

template <typename T, meta::require<std::is_same<meta::remove_cv<T>, wchar_t>::value> = 0>
inline constexpr wchar_t null_character() noexcept { return L'\0'; }

template <typename T, meta::require<std::is_same<meta::remove_cv<T>, char16_t>::value> = 0>
inline constexpr char16_t null_character() noexcept { return u'\0'; }

template <typename T, meta::require<std::is_same<meta::remove_cv<T>, char32_t>::value> = 0>
inline constexpr char32_t null_character() noexcept { return U'\0'; }

template <typename CharType, meta::require<meta::is_character<CharType>()> = 0>
std::size_t size(const CharType* str) noexcept
{
    std::size_t count = 0;
    while(*str++ != null_character<CharType>()) ++count;

    return count;
}

} // namespace utility

} // namespace sifar

#endif // SIFAR_DETAIL_TOOLS_HPP
