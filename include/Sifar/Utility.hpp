#ifndef SIFAR_UTILITY_HPP
#define SIFAR_UTILITY_HPP

#include <cstdint> // int32_t, uint32_t, int64_t, uint64_t

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define _NULL_CHARACTER_FUNCTION_GENERIC(char_type, char_value)                                         \
    template <typename T, SIREQUIRE(std::is_same<meta::remove_cv<T>, char_type>::value)>                \
    inline constexpr char_type null_character() noexcept { return char_value; }

namespace sifar
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

template <typename T> const char* const_byte_cast(T* data) noexcept
{
    return reinterpret_cast<const char*>(data);
}

template <typename T> char* byte_cast(T* data) noexcept
{
    return reinterpret_cast<char*>(data);
}

template <typename T, SIREQUIRE(not meta::is_polymorphic<T>())>
void* pure(T* pointer)
{
    return static_cast<void*>(pointer);
}

template <typename T, SIREQUIRE(meta::is_polymorphic<T>())>
void* pure(T* pointer_to_polymorphic)
{
    return dynamic_cast<void*>(pointer_to_polymorphic);
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

_NULL_CHARACTER_FUNCTION_GENERIC(char, '\0')
_NULL_CHARACTER_FUNCTION_GENERIC(wchar_t, L'\0')
_NULL_CHARACTER_FUNCTION_GENERIC(char16_t, u'\0')
_NULL_CHARACTER_FUNCTION_GENERIC(char32_t, U'\0')

template <typename CharType, SIREQUIRE(meta::is_character<CharType>())>
std::size_t size(const CharType* str) noexcept
{
    std::size_t count = 0;
    while(*str++ != null_character<CharType>()) ++count;

    return count;
}

} // namespace utility

} // namespace sifar

// clean up
#undef _NULL_CHARACTER_FUNCTION_GENERIC

#endif // SIFAR_UTILITY_HPP
