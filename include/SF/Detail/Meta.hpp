#ifndef SF_DETAIL_META_HPP
#define SF_DETAIL_META_HPP

#include <cstddef> // size_t

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#include <memory> // shared_ptr

#include <SF/Core/SerializatonBase.hpp>

#include <SF/Detail/MetaMacro.hpp> // SFVOID

#ifndef SF_MAX_TEMPLATE_DEPTH
    #define SF_MAX_TEMPLATE_DEPTH 256
#endif // SF_MAX_TEMPLATE_DEPTH

namespace sf
{

namespace meta
{

struct shared_type {};
struct raw_type {};

struct dummy_type {};

template <std::size_t N>
struct constant_index : std::integral_constant<std::size_t, N> {};

template <bool condition, typename T = void>
using when = typename std::enable_if<condition, T>::type;

template <bool condition>
using require = when<condition, int>;

template <typename... Args>
using to_void = void;

template <typename T>
using remove_ptr = typename std::remove_pointer<T>::type;

template <typename T>
using remove_cv = typename std::remove_cv<T>::type;

template <typename T>
using remove_ref = typename std::remove_reference<T>::type;

namespace detail
{

template <typename T, typename = void>
struct deref_implementation { using type = T; };

template <>
struct deref_implementation<void*> { using type = void; };

template <typename T>
struct deref_implementation<std::weak_ptr<T>> { using type = T; };

template <typename T>
struct deref_implementation<T, SFVOID(*std::declval<T>())>
{
    using type = remove_ref<decltype(*std::declval<T>())>;
};

} // namespace detail

template <typename It>
using dereference = typename detail::deref_implementation<It>::type;

template <bool condition, typename if_true, typename if_false>
using conditional = typename std::conditional<condition, if_true, if_false>::type;

template <typename T>
using decay = typename std::decay<T>::type;

template <typename...> struct type_array;

template <> struct type_array<>
{
    template <int I> struct arg { using type = void; };
};

template <typename T, typename... Tn>
struct type_array<T, Tn...>
{
    template <int I, typename overload = void> struct arg
    {
        using type = typename type_array<Tn...>::template arg<I - 1>::type;
    };

    template <typename overload> struct arg<0, overload>
    {
        using type = T;
    };

    template <int I> using type = typename arg<I>::type;
};

namespace detail
{

template <class...> struct all : std::true_type {};
template <class B1> struct all<B1> : B1 {};
template <class B1, class... Bn>
struct all<B1, Bn...> : conditional<bool(B1::value), all<Bn...>, B1> {};

template <class...> struct one : std::false_type {};
template <class B1> struct one<B1> : B1 {};
template <class B1, class... Bn>
struct one<B1, Bn...> : conditional<bool(B1::value), B1, one<Bn...>> {};

} // namespace detaiil

template <typename... Bn> constexpr bool all() noexcept
{
    return detail::all<Bn...>::value;
}

template <typename... Bn> constexpr bool one() noexcept
{
    return detail::one<Bn...>::value;
}

namespace detail
{

template <typename From, typename To, typename = void>
struct can_static_cast_implementation : std::false_type {};

template <typename From, typename To>
struct can_static_cast_implementation<From, To,
    SFVOID( static_cast<To*>(std::declval<From*>()) )>
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
struct remove_ptr_n_implementation;

template <typename T>
struct remove_ptr_n_implementation<T*, 1>
{
    using type = T;
};

template <typename T, std::size_t N>
struct remove_ptr_n_implementation<T*, N>
{
    using type = typename remove_ptr_n_implementation<T, N - 1>::type;
};

} // namespace detail

template <typename T, std::size_t N = 1>
using remove_ptr_n = typename detail::remove_ptr_n_implementation<T, N>::type;

namespace detail
{

template <typename T, std::size_t N>
struct pointer_implementation
{
    using type = typename pointer_implementation<T, N - 1>::type*;
};

template <typename T>
struct pointer_implementation<T, 0>
{
    using type = T;
};

} // namespace detail

template <typename T, std::size_t N>
using pointer = typename detail::pointer_implementation<T, N>::type;

template <typename> struct is_std_shared_ptr : std::false_type {};
template <typename T>
struct is_std_shared_ptr<std::shared_ptr<T>> : std::true_type {};

namespace detail
{

template <typename>
struct is_character_implementation : std::false_type {};

template <> struct is_character_implementation<char> : std::true_type {};
template <> struct is_character_implementation<signed char> : std::true_type {};
template <> struct is_character_implementation<unsigned char> : std::true_type {};

template <> struct is_character_implementation<wchar_t> : std::true_type {};
template <> struct is_character_implementation<char16_t> : std::true_type {};
template <> struct is_character_implementation<char32_t> : std::true_type {};

template <typename T>
struct is_character : is_character_implementation<typename std::remove_cv<T>::type> {};

template <typename T, typename... Tn>
struct is_same_all : all<std::is_same<T, Tn>...> {};

template <typename T>
struct is_same_all<T> : std::true_type {};

} // namespace detail

 // limited by template depth
template <std::size_t... I>
struct index_sequence
{
    static constexpr std::size_t size() noexcept { return sizeof...(I); }
};

namespace detail
{

template <std::size_t I, std::size_t... In>
struct index_sequence_helper : index_sequence_helper<I - 1, I - 1, In...> {};

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
    return std::size_t(SF_MAX_TEMPLATE_DEPTH);
}

namespace detail
{

template <class T, typename enable = meta::to_void<>>
struct object_value_type_implementation
{
    using type = dummy_type;
};

template <class T>
struct object_value_type_implementation<T, meta::to_void<typename T::value_type>>
{
    using type = typename T::value_type;
};

template <typename T>
struct array_value_type_implementation
{
    using type = dummy_type;
};

template <typename T>
struct array_value_type_implementation<T[]>
{
    using type = T;
};

template <typename T, std::size_t N>
struct array_value_type_implementation<T[N]>
{
    using type = T;
};

template <typename T, typename enable = void>
struct value_type_implementation
{
    using type = dummy_type;
};

template <typename T>
struct value_type_implementation<T, when<std::is_class<T>::value>>
{
    using type = typename object_value_type_implementation<T>::type;
};

template <typename T>
struct value_type_implementation<T, when<std::is_array<T>::value>>
{
    using type = typename array_value_type_implementation<T>::type;
};

} // namespace detail

template <typename T>
using value_type = typename detail::value_type_implementation<T>::type;

template <typename T> constexpr std::size_t pointer_count() noexcept
{
    return std::is_pointer<T>::value
         ? 1 + pointer_count<meta::remove_ptr<T>>()
         : 0;
}

namespace detail
{

template <typename> struct is_read_archive : std::false_type {};

template <class InStream, class StreamWrapper, class Registry>
struct is_read_archive<ReadArchive<InStream, StreamWrapper, Registry>> : std::true_type {};

} // namespace detail

template <class T> constexpr bool is_read_archive() noexcept
{
    return detail::is_read_archive<T>::value;
}

namespace detail
{

template <typename> struct is_write_archive : std::false_type {};

template <class OutStream, class StreamWrapper, class Registry>
struct is_write_archive<WriteArchive<OutStream, Registry, StreamWrapper>> : std::true_type {};

} // namespace detail

template <class T> constexpr bool is_write_archive() noexcept
{
    return detail::is_write_archive<T>::value;
}

namespace detail
{

template <typename> struct is_base_archive : std::false_type {};

template <>
struct is_base_archive<core::ArchiveBase> : std::true_type {};

} // namespace detail

template <class T> constexpr bool is_base_archive() noexcept
{
    return detail::is_base_archive<T>::value;
}

template <class T> constexpr bool is_archive() noexcept
{
    return is_read_archive<T>() or is_write_archive<T>();
}

template <class T> constexpr bool is_Save() noexcept { return is_write_archive<T>(); }
template <class T> constexpr bool is_Load() noexcept { return is_read_archive<T>(); }
template <class T> constexpr bool is_SaveLoad() noexcept { return is_archive<T>(); }

template <std::size_t I> constexpr std::size_t with() noexcept
{
    return I;
}

template <std::size_t I = 0, typename B, typename... Bn>
constexpr std::size_t with(B b, Bn... bn) noexcept
{
    return b ? I : with<I + 1>(bn...);
}

template <typename... Tn> constexpr bool to_false() noexcept { return false; }

template <class T> constexpr bool is_class() noexcept
{
    return std::is_class<T>::value;
}

template <class T> constexpr bool is_compressible() noexcept
{
    return std::is_arithmetic<value_type<T>>::value;
}

template <typename From, typename To> constexpr bool can_static_cast() noexcept
{
    return detail::can_static_cast_implementation<From, To>::value;
}

template <typename From, typename To> constexpr bool is_cast_allowed() noexcept
{
    return can_static_cast<From, To>() or std::is_convertible<From, To>::value;
}

template <class Base, class Derived> constexpr bool is_base_of() noexcept
{
    return std::is_base_of<Base, Derived>::value;
}

template <class Derived, class Base, class... Base_n> constexpr bool is_derived_of() noexcept
{
    return meta::all<std::is_base_of<Base, Derived>,
                     std::is_base_of<Base_n, Derived>...>();
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

template <typename T> constexpr bool is_void() noexcept
{
    return std::is_same<T, void>::value;
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

template <typename T> constexpr bool is_shared_pointer() noexcept
{
    return is_std_shared_ptr<T>::value;
}

template <typename T> constexpr bool is_raw_pointer() noexcept
{
    return std::is_pointer<T>::value;
}

template <typename T> constexpr bool is_pointer() noexcept
{
    return is_raw_pointer<T>() or is_shared_pointer<T>();
}

template <typename T> constexpr bool is_pointer_to_abstract() noexcept
{
    return is_pointer<T>() and std::is_abstract<meta::dereference<T>>::value;
}

template <typename T> constexpr bool is_pointer_to_polymorphic() noexcept
{
    return is_pointer<T>() and std::is_polymorphic<meta::dereference<T>>::value;
}

template <typename T> constexpr bool is_void_pointer() noexcept
{
    return is_pointer<T>() and is_void<dereference<T>>();
}

template <typename T> constexpr bool is_null_pointer() noexcept
{
    return std::is_same<T, std::nullptr_t>::value;
}

template <typename T> constexpr bool is_function_pointer() noexcept
{
    return detail::is_function_pointer<T>();
}

template <typename T> constexpr bool is_pointer_to_standard_layout() noexcept
{
    return is_pointer<T>() and
       not is_void_pointer<T>() and
       not is_pointer_to_polymorphic<T>() and
       not is_function_pointer<T>() and
       not is_null_pointer<T>() and
       not std::is_member_pointer<T>::value;
}

template <typename T> constexpr bool is_serializable_pointer() noexcept
{
    return is_pointer_to_standard_layout<T>() or is_pointer_to_polymorphic<T>();
}

template <typename T> constexpr bool is_serializable_raw_pointer() noexcept
{
    return is_raw_pointer<T>() and is_serializable_pointer<T>();
}

template <typename T> constexpr bool is_serializable_shared_pointer() noexcept
{
    return is_shared_pointer<T>() and is_serializable_pointer<T>();
}

template <typename T> constexpr bool is_unsupported() noexcept
{
    return is_void_pointer<T>()
        or is_function_pointer<T>()
        or is_null_pointer<T>()
        or std::is_function<T>::value
        or std::is_member_function_pointer<T>::value
        or std::is_member_object_pointer<T>::value
        or std::is_reference<T>::value;
}

} // namespace meta

} // namespace sf

#endif // SF_DETAIL_META_HPP