#ifndef SIFAR_DETAIL_META_HPP
#define SIFAR_DETAIL_META_HPP

#include <cstddef> // size_t
#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#include <Sifar/Detail/MacroScope.hpp>

namespace sifar
{

namespace meta
{

template <bool condition, typename T = void>
using when = typename std::enable_if<condition, T>::type;

template <bool condition>
using require = when<condition, int>;

template <typename... Args>
using void_t = void;

template <typename T>
using remove_ptr = typename std::remove_pointer<T>::type;

template <typename T>
using remove_cv = typename std::remove_cv<T>::type;

template <typename T>
using remove_ref = typename std::remove_reference<T>::type;

namespace detail
{

template <typename T, typename = void>
struct deref_impl { using type = T; };

template <>
struct deref_impl<void*>
{
    using type = void;
};

template <typename T>
struct deref_impl<T, void_t<decltype(*std::declval<T>())>>
{
    using type = remove_ref<decltype(*std::declval<T>())>;
};

} // namespace detail

template <typename It>
using deref = typename detail::deref_impl<It>::type;

template <bool condition, typename if_true, typename if_false>
using if_ = typename std::conditional<condition, if_true, if_false>::type;

template <typename T>
using decay = typename std::decay<T>::type;

template <class...> struct and_ : std::true_type {};
template <class B1> struct and_<B1> : B1 {};
template <class B1, class... Bn>
struct and_<B1, Bn...>
    : if_<bool(B1::value), and_<Bn...>, B1> {};

namespace detail
{

template <typename From, typename To, typename = void>
struct can_static_cast: std::false_type {};

template <typename From, typename To>
struct can_static_cast<From, To,
    void_t<decltype( static_cast<To>(std::declval<From>()) )>>
: std::true_type {};

} // namespace detail

namespace detail
{

template <typename>
struct is_function_pointer : std::false_type {};

template <typename Ret, typename... Args>
struct is_function_pointer<Ret (*)(Args...)> : std::true_type {};

} // namespace detail

namespace detail
{

template <typename, std::size_t N>
struct remove_ptr_n_impl;

template <typename T>
struct remove_ptr_n_impl<T*, 1>
{
    using type = T;
};

template <typename T, std::size_t N>
struct remove_ptr_n_impl<T*, N>
{
    using type = typename remove_ptr_n_impl<T, N - 1>::type;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using remove_ptr_n = typename detail::remove_ptr_n_impl<T, N>::type;

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

template <std::size_t...>
struct index_sequence {}; // limited by template depth

namespace detail
{

template <std::size_t I, std::size_t... In>
struct index_sequence_helper : public index_sequence_helper<I - 1, I - 1, In...> {};

template <std::size_t... In>
struct index_sequence_helper<0, In...>
{
    using type = index_sequence<In...>;
};

} // namespace detail

template <std::size_t N>
using make_index_sequence = typename detail::index_sequence_helper<N>::type;

constexpr std::size_t max_template_depth() noexcept
{
#ifndef SIFAR_MAX_TEMPLATE_DEPTH
    return 256;
#else
    return SIFAR_MAX_TEMPLATE_DEPTH;
#endif
}

template <typename T> constexpr bool to_false() noexcept { return false; }

template <class Base, class Derived> constexpr bool is_base_of() noexcept
{
    return std::is_base_of<Base, Derived>::value;
}

template <class Base, class Derived> constexpr bool is_virtual_base_of() noexcept
{
    return is_base_of<Base, Derived>()
       and not detail::can_static_cast<Base*, Derived*>::value;
}

template <typename T> constexpr bool is_abstract() noexcept
{
    return std::is_abstract<T>::value;
}

template <typename T> constexpr bool is_polymorphic() noexcept
{
    return std::is_polymorphic<T>::value;
}

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

template <typename T> constexpr bool is_pointer_to_abstract() noexcept
{
    return is_pointer<T>() and std::is_abstract<meta::deref<T>>::value;
}

template <typename T> constexpr bool is_pointer_to_polymorphic() noexcept
{
    return is_pointer<T>() and std::is_polymorphic<meta::deref<T>>::value;
}

template <typename T> constexpr bool is_void_pointer() noexcept
{
    return std::is_same<T, void*>::value;
}

template <typename T> constexpr bool is_function_pointer() noexcept
{
    return detail::is_function_pointer<T>();
}

template <typename T> constexpr bool is_pod_pointer() noexcept
{
    return is_pointer<T>()
       and not is_void_pointer<T>()
       and not is_pointer_to_polymorphic<T>()
       and not is_function_pointer<T>()
       and not std::is_null_pointer<T>()
       and not std::is_member_pointer<T>::value;
}

template <typename T> constexpr bool is_unsupported() noexcept
{
    return is_void_pointer<T>()
        or is_function_pointer<T>()
        or std::is_null_pointer<T>::value
        or std::is_function<T>::value
        or std::is_member_function_pointer<T>::value
        or std::is_member_object_pointer<T>::value;
}

} // namespace meta

} // namespace sifar

#include <Sifar/Detail/MacroUnscope.hpp>

#endif // SIFAR_DETAIL_META_HPP
