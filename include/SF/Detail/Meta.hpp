﻿#ifndef SF_DETAIL_META_HPP
#define SF_DETAIL_META_HPP

#include <cstddef> // size_t

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#if __cplusplus >= 201703L
#include <any> // any
#endif // if

#include <memory> // shared_ptr
#include <array> // array

#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace meta
{

template <typename... Tn> constexpr bool to_false() noexcept { return false; }

template <typename T, std::size_t I = 0>
auto declval() noexcept -> decltype(std::declval<T>()) { return std::declval<T>(); }

template <class...> struct all : std::true_type {};
template <class B1> struct all<B1> : B1 {};
template <class B1, class... Bn>
struct all<B1, Bn...> : std::conditional<bool(B1::value), all<Bn...>, B1>::type {};

template <class...> struct one : std::false_type {};
template <class B1> struct one<B1> : B1 {};
template <class B1, class... Bn>
struct one<B1, Bn...> : std::conditional<bool(B1::value), B1, one<Bn...>>::type {};

template <class B> struct negation : std::integral_constant<bool, not bool(B::value)> {};

template <typename T, typename... Tn> struct is_same : all<std::is_same<T, Tn>...> {};

template <typename... Args> using void_t = void;

template <typename T, std::size_t N = 1> struct remove_pointer { using type = T; };
template <typename T> struct remove_pointer<T*, 1> { using type = T; };

template <typename T, std::size_t N>
struct remove_pointer<T*, N> { using type = typename remove_pointer<T, N - 1>::type; };

template <typename T, typename = void_t<>> struct dereference { using type = T; };
template <typename T> struct dereference<T*> { using type = T; };
template <typename T> struct dereference<std::weak_ptr<T>> { using type = T; };
template <typename T> struct dereference<std::shared_ptr<T>> { using type = T; };
template <typename T> struct dereference<std::unique_ptr<T>> { using type = T; };
template <typename T> struct dereference<T, void_t<decltype(*std::declval<T>())>>
    : std::remove_reference<decltype(*std::declval<T>())> {};

template <typename T, std::size_t N = 1>
struct pointer { using type = typename pointer<T, N - 1>::type*; };
template <typename T> struct pointer<T, 0> { using type = T; };

 // limited by template depth
template <std::size_t... I> struct index_sequence
{
    static constexpr auto value = sizeof...(I);
};

namespace detail
{

template <std::size_t I, std::size_t... In>
struct index_sequence_helper : index_sequence_helper<I - 1, I - 1, In...> {};
template <std::size_t... In>
struct index_sequence_helper<0, In...> { using type = index_sequence<In...>; };

} // namespace detail

template <std::size_t N>
using make_index_sequence = typename detail::index_sequence_helper<N>::type;

// meta
template <typename T, typename enable = void> struct is_complete : std::false_type {};
template <typename T> struct is_complete<T, void_t<decltype(sizeof(T))>> : std::true_type {};

template <typename From, typename To, typename enable = void>
struct is_static_castable : std::false_type {};

template <typename From, typename To>
struct is_static_castable<From, To, void_t<decltype( static_cast<To>(std::declval<From>()) )>> : std::true_type {};

template <typename T, bool = std::is_pointer<T>::value>
struct pointer_count
{
    static constexpr auto value = pointer_count<typename remove_pointer<T>::type>::value + 1;
};

template <typename T>
struct pointer_count<T, false>
{
    static constexpr auto value = std::size_t(0);
};

struct shared_common_t {};
struct raw_common_t {};

struct dummy_t
{
#if __cplusplus >= 201703L
    template <typename T, SF_REQUIRE(not std::is_same<T, std::any>::value)> operator T();
#else
    template <typename T> operator T();
#endif // if
};

#if __cplusplus >= 201703L
template <class C, typename S = index_sequence<>, typename overload = void>
struct aggregate_size : S {};

template <class C, std::size_t... I>
struct aggregate_size<C, index_sequence<I...>,
                      void_t<decltype(C{ declval<dummy_t>(), declval<dummy_t, I>()... })>>
    : aggregate_size<C, index_sequence<I..., sizeof...(I)>> {};
#endif // if

template <class T, typename enable = void>
struct object_value { using type = dummy_t; };

template <class T>
struct object_value<T, void_t<typename T::value_type>> { using type = typename T::value_type; };

template <typename T> struct array_value { using type = dummy_t; };
template <typename T> struct array_value<T[]> { using type = T; };
template <typename T, std::size_t N> struct array_value<T[N]> { using type = T; };

template <typename T, typename enable = void>
struct value { using type = dummy_t; };

template <typename T>
struct value<T, typename std::enable_if<std::is_class<T>::value>::type>
{
    using type = typename object_value<T>::type;
};

template <typename T>
struct value<T, typename std::enable_if<std::is_array<T>::value>::type>
{
    using type = typename array_value<T>::type;
};

template <typename From, typename To> struct is_cast_allowed
    : one<is_static_castable<From, To>, std::is_convertible<From, To>> {};

template <class T> struct is_compressible : std::is_arithmetic<typename value<T>::type> {};

template <class Derived, class Base, class... Base_n> struct is_derived_of
    : all<std::is_base_of<Base, Derived>,
          std::is_base_of<Base_n, Derived>...> {};

template <class Base, class Derived> struct is_virtual_base_of
    : all<std::is_base_of<Base, Derived>,
          negation<is_static_castable<Base*, Derived*>>> {};

template <typename> struct is_std_shared_ptr : std::false_type {};
template <typename T>
struct is_std_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename T> struct is_shared_pointer : is_std_shared_ptr<T> {};
template <typename T> struct is_raw_pointer : std::is_pointer<T> {};

template <typename T> struct is_pointer : one<is_shared_pointer<T>, is_raw_pointer<T>> {};

template <typename T> struct is_pointer_to_polymorphic
    : all<is_pointer<T>, std::is_polymorphic<typename dereference<T>::type>> {};

template <typename T> struct is_void_pointer : all<is_pointer<T>, std::is_void<typename dereference<T>::type>> {};
template <typename T> struct is_null_pointer : std::is_same<T, std::nullptr_t> {};

template <typename> struct is_function_pointer : std::false_type {};
template <typename Ret, typename... Args>
struct is_function_pointer<Ret (*)(Args...)> : std::true_type {};

template <typename T> struct is_pointer_to_standard_layout :
    all<is_pointer<T>,
        negation<is_void_pointer<T>>,
        negation<is_pointer_to_polymorphic<T>>,
        negation<is_function_pointer<T>>,
        negation<is_null_pointer<T>>,
        negation<std::is_member_pointer<T>>> {};

template <typename T> struct is_serializable_pointer
    : one<is_pointer_to_standard_layout<T>, is_pointer_to_polymorphic<T>> {};

template <typename T> struct is_serializable_raw_pointer
    : all<is_raw_pointer<T>, is_serializable_pointer<T>> {};

template <typename T> struct is_serializable_shared_pointer
    : all<is_shared_pointer<T>, is_serializable_pointer<T>> {};

#if __cplusplus >= 201703L
template <typename T> struct is_aggregate
    : all<std::is_aggregate<T>, negation<is_std_array<T>>, negation<std::is_array<T>>> {};
#endif // if

template <typename T> struct is_unsupported :
    one<is_void_pointer<T>,
        is_function_pointer<T>,
        is_null_pointer<T>,
        std::is_function<T>,
        std::is_member_function_pointer<T>,
        std::is_member_object_pointer<T>,
        std::is_reference<T>> {};

} // namespace meta

} // namespace sf

#endif // SF_DETAIL_META_HPP
