#ifndef SERIALIZATION_META_HPP
#define SERIALIZATION_META_HPP

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#include "MacroScope.hpp"

namespace serialization
{

namespace meta
{

template <bool condition, typename T = void>
using when = typename std::enable_if<condition, T>::type;

template <bool condition>
using require = when<condition, int>;

template <typename... Args>
using void_t = void;

template <typename It>
using deref = typename std::remove_reference<decltype(*std::declval<It>())>::type;

template <typename T>
using remove_cv = typename std::remove_cv<T>::type;

template <bool condition, typename if_true, typename if_false>
using if_ = typename std::conditional<condition, if_true, if_false>::type;

template <class...> struct and_ : std::true_type {};
template <class B1> struct and_<B1> : B1 {};
template <class B1, class... Bn>
struct and_<B1, Bn...>
    : if_<bool(B1::value), and_<Bn...>, B1> {};

namespace detail
{

template <typename, std::size_t N>
struct remove_pointer_impl;

template <typename T>
struct remove_pointer_impl<T*, 1>
{
    using type = T;
};

template <typename T, std::size_t N>
struct remove_pointer_impl<T*, N>
{
    using type = typename remove_pointer_impl<T, N - 1>::type;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using remove_pointer = typename detail::remove_pointer_impl<T, N>::type;

namespace detail
{

template <typename>
struct is_character_impl : std::false_type {};

template <> struct is_character_impl<char> : std::true_type {};
template <> struct is_character_impl<signed char> : std::true_type {};
template <> struct is_character_impl<unsigned char> : std::true_type {};

template <> struct is_character_impl<wchar_t> : std::true_type {};
template <> struct is_character_impl<char16_t> : std::true_type {};
template <> struct is_character_impl<char32_t> : std::true_type {};

template <typename T>
struct is_character : is_character_impl<typename std::remove_cv<T>::type> {};

template <class T, class... Tn>
struct is_same_all: and_<std::is_same<T, Tn>...> {};

} // namespace detail

template <typename T, typename... Tn> constexpr bool is_same_all() noexcept
{
    return detail::is_same_all<T, Tn...>::value;
}

template <typename T> constexpr bool is_character() noexcept
{
    return detail::is_character<T>::value;
}

template <typename T> constexpr bool is_arithmetic() noexcept
{
    return std::is_arithmetic<T>::value;
}

template <typename T> constexpr bool is_enum() noexcept
{
    return std::is_enum<T>::value;
}

template <typename T> constexpr bool is_array() noexcept
{
    return std::is_array<T>::value;
}

template <typename T> constexpr bool is_pointer() noexcept
{
    return std::is_pointer<T>::value;
}

} // namespace meta

} // namespace serialization

#include "MacroUnscope.hpp"

#endif // SERIALIZATION_META_HPP
