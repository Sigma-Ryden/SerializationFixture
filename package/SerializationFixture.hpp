#ifndef SERIALIZATION_FIXTURE_HPP
#define SERIALIZATION_FIXTURE_HPP

namespace sf
{

namespace core
{

#ifndef SF_TYPE_PORTABLE_DISABLE

static_assert(sizeof(char) == 1, "Require character size.");

static_assert(sizeof(float) == 4, "Require floating point number size.");
static_assert(sizeof(double) == 8, "Require double precision floation point number size.");

static_assert(sizeof(int) == 4, "Require integer number size.");

#endif // SF_TYPE_PORTABLE_DISABLE

} // namespace core

} // namespace sf

#include <cstddef> // size_t

#include <type_traits>
// is_enum, is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#if __cplusplus >= 201703L
#include <any> // any
#endif // if

#include <memory> // shared_ptr
#include <array> // array

#define SF_REQUIRE(...) typename std::enable_if<(bool)(__VA_ARGS__), int>::type = 0

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

namespace sf
{

namespace core
{

struct ioarchive_t;

struct iarchive_common_t {};
struct oarchive_common_t {};

} // namespace core

namespace meta
{

template <class T> struct is_iarchive : std::is_base_of<core::iarchive_common_t, T> {};
template <class T> struct is_oarchive : std::is_base_of<core::oarchive_common_t, T> {};

template <class T> struct is_ioarchive : one<is_iarchive<T>, is_oarchive<T>> {};

} // namespace meta

} // namespace sf

#include <cstdint> // int8_t, uint8_t, int32_t, uint32_t, int64_t, uint64_t

namespace sf
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

} // namespace sf

#ifndef SF_ARCHIVE_TRAIT_MAX_KEY_SIZE
    #define SF_ARCHIVE_TRAIT_MAX_KEY_SIZE 3
#endif // SF_ARCHIVE_TRAIT_MAX_KEY_SIZE

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                \
    template <> struct xxsf_archive_traits<__VA_ARGS__> {                                           \
        static constexpr auto key = ::xxsf_archive_traits_key_type(archive_key);                    \
    };                                                                                              \
    template <> struct xxsf_##archive_type##archive_registry<archive_key> {                         \
        using type = __VA_ARGS__;                                                                   \
    };                                                                                              \

namespace sf
{

namespace core
{

struct ioarchive_t;

} // namespace core

} // namespace sf

using xxsf_archive_traits_key_type = ::sf::let::u8;
static constexpr auto xxsf_archive_traits_base_key = xxsf_archive_traits_key_type(-1);

template <class ArchiveType>
struct xxsf_archive_traits
{
    static constexpr auto key = xxsf_archive_traits_base_key;
};

template <xxsf_archive_traits_key_type ArchiveKey>
struct xxsf_iarchive_registry { using type = ::sf::core::ioarchive_t; };

template <xxsf_archive_traits_key_type ArchiveKey>
struct xxsf_oarchive_registry { using type = ::sf::core::ioarchive_t; };

namespace sf
{

namespace core
{

struct ioarchive_t
{
    ioarchive_t(::xxsf_archive_traits_key_type trait = ::xxsf_archive_traits_base_key, bool readonly = false)
        : trait(trait), readonly(readonly) {}

#ifdef SF_DEBUG
    virtual ~ioarchive_t() = default;
#endif // SF_DEBUG

    const ::xxsf_archive_traits_key_type trait;
    const bool readonly;
};

} // namespace core

} // namespace sf

#include <typeinfo> // type_info

#ifndef SF_STATIC_HASH_KEY_TYPE
    #define SF_STATIC_HASH_KEY_TYPE ::sf::let::u64
#endif // SF_STATIC_HASH_KEY_TYPE

#ifndef SF_STATIC_HASH
    #define SF_STATIC_HASH(string) ::sf::static_hash<SF_STATIC_HASH_KEY_TYPE>((string))
#endif // SF_STATIC_HASH

#ifndef SF_TYPE_HASH
    #define SF_TYPE_HASH(type_info) ::sf::type_hash(type_info)
#endif // SF_TYPE_HASH

namespace sf
{

namespace detail
{

enum class Word { x32, x64 }; // word size: x* - number of bits

template <Word word> struct word_type_impl;

template <> struct word_type_impl<Word::x32> { using type = let::u32; };
template <> struct word_type_impl<Word::x64> { using type = let::u64; };

template <Word word>
using word_t = typename detail::word_type_impl<word>::type;

template <typename T> struct word_traits { static constexpr auto value = Word::x64; };

template <> struct word_traits<let::u32> { static constexpr auto value = Word::x32; };
template <> struct word_traits<let::u64> { static constexpr auto value = Word::x64; };

template <typename HashType, HashType FnvPrime, HashType OffsetBasis>
HashType fnv_1a(const char* text)
{
    auto hash = OffsetBasis;
    while (*text != '\0')
    {
        hash ^= *text;
        hash *= FnvPrime;

        ++text;
    }

    return hash;
}

template <typename HashType, HashType FnvPrime, HashType OffsetBasis>
constexpr HashType static_fnv_1a(const char* text, HashType hash = OffsetBasis) noexcept
{
    return (*text == '\0')
           ? hash
           : static_fnv_1a<HashType, FnvPrime, OffsetBasis>(text + 1, (hash ^ (*text)) * FnvPrime);
}

template <detail::Word word>
struct hash_t;

template <>
struct hash_t<detail::Word::x32>
{
private:
    // For 32 bit machines:
    static constexpr let::u32 fnv_prime        = 16777619ull;
    static constexpr let::u32 fnv_offset_basis = 2166136261ull;

public:
    static let::u32 run(const char* text)
    {
        return detail::fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u32 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct hash_t<detail::Word::x64>
{
private:
    // For 64 bit machines:
    static constexpr let::u64 fnv_prime        = 1099511628211ull;
    static constexpr let::u64 fnv_offset_basis = 14695981039346656037ull;

public:
    static let::u64 run(const char* text)
    {
        return detail::fnv_1a<let::u64, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u64 static_run(const char* text) noexcept
    {
        return detail::static_fnv_1a<let::u64, fnv_prime, fnv_offset_basis>(text);
    }
};

} // namespace detail

template <typename key_type = let::u64>
inline key_type hash(const char* text)
{
    using detail::hash_t;
    using detail::word_traits;

    return hash_t<detail::word_traits<key_type>::value>::run(text);
}

template <typename key_type = let::u64>
constexpr key_type static_hash(const char* text) noexcept
{
    using detail::hash_t;
    using detail::word_traits;

    return hash_t<detail::word_traits<key_type>::value>::static_run(text);
}

inline std::size_t type_hash(const std::type_info& type) noexcept
{
    // not portable impl - will be changed
    return type.hash_code();
}

namespace detail
{

template <typename HashType, typename T>
inline void hash_combine(HashType& seed, const T& object) noexcept
{
    auto hash = std::hash<T>{}(object) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<HashType>(hash);
}

} // namespace detail

} // namepace sf

#define EXPORT_INSTANTIABLE_KEY(name, ...)                                                              \
    template <> struct xxsf_instantiable_traits<__VA_ARGS__> {                                          \
        static constexpr auto key = SF_STATIC_HASH(name);                                               \
    };

#define EXPORT_INSTANTIABLE(...)                                                                        \
    EXPORT_INSTANTIABLE_KEY(#__VA_ARGS__, __VA_ARGS__)

using xxsf_instantiable_traits_key_type = SF_STATIC_HASH_KEY_TYPE;
static constexpr auto xxsf_instantiable_traits_base_key = xxsf_instantiable_traits_key_type(-1);

template <class InstantiableType>
struct xxsf_instantiable_traits
{
    static constexpr auto key = xxsf_instantiable_traits_base_key;
};

// concatenation of two macro arguments
#define SF_CONCAT(lhs, rhs) SF_CONCAT_IMPL(lhs, rhs)
#define SF_CONCAT_IMPL(lhs, rhs) SF_CONCAT_IMPL_(lhs, rhs)
#define SF_CONCAT_IMPL_(lhs, rhs) lhs##rhs

// deparen of macro argument
#define SF_DEPAREN(arg) SF_DEPAREN_IMPL(SF_UATE arg)
#define SF_UATE(...) SF_UATE __VA_ARGS__
#define SF_DEPAREN_IMPL(...) SF_DEPAREN_IMPL_(__VA_ARGS__)
#define SF_DEPAREN_IMPL_(...) SF_EVAL ## __VA_ARGS__
#define SF_EVALSF_UATE

// return first argument from two
#define SF_FIRST_ARGUMENT(first, ...) first

#define SF_VA_ARGS_SIZE_IMPL_(                                                                          \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, _11, _12, _13, _14, _15, _16,                     \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,                     \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,                     \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N

#define SF_REVERSE_INTEGER_SEQUENCE                                                                     \
    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48,                                     \
    47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32,                                     \
    31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,                                     \
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define SF_VA_ARGS_SIZE_IMPL(...) SF_VA_ARGS_SIZE_IMPL_(__VA_ARGS__)

// return __VA_ARGS__ arguments count, for empty __VA_ARGS__ return 1
#define SF_VA_ARGS_SIZE(...) SF_VA_ARGS_SIZE_IMPL(__VA_ARGS__, SF_REVERSE_INTEGER_SEQUENCE)

// generate repeat sequence macro(1) macro(2) ... macro(count)
#define SF_REPEAT(macro, count) SF_CONCAT(SF_REPEAT_, count)(macro)

#define SF_REPEAT_0(macro)

#define SF_REPEAT_1(macro) SF_REPEAT_0(macro) macro(1)
#define SF_REPEAT_2(macro) SF_REPEAT_1(macro) macro(2)
#define SF_REPEAT_3(macro) SF_REPEAT_2(macro) macro(3)
#define SF_REPEAT_4(macro) SF_REPEAT_3(macro) macro(4)
#define SF_REPEAT_5(macro) SF_REPEAT_4(macro) macro(5)
#define SF_REPEAT_6(macro) SF_REPEAT_5(macro) macro(6)
#define SF_REPEAT_7(macro) SF_REPEAT_6(macro) macro(7)
#define SF_REPEAT_8(macro) SF_REPEAT_7(macro) macro(8)

#define SF_REPEAT_9(macro) SF_REPEAT_8(macro) macro(9)
#define SF_REPEAT_10(macro) SF_REPEAT_9(macro) macro(10)
#define SF_REPEAT_11(macro) SF_REPEAT_10(macro) macro(11)
#define SF_REPEAT_12(macro) SF_REPEAT_11(macro) macro(12)
#define SF_REPEAT_13(macro) SF_REPEAT_12(macro) macro(13)
#define SF_REPEAT_14(macro) SF_REPEAT_13(macro) macro(14)
#define SF_REPEAT_15(macro) SF_REPEAT_14(macro) macro(15)
#define SF_REPEAT_16(macro) SF_REPEAT_15(macro) macro(16)

#define SF_REPEAT_17(macro) SF_REPEAT_16(macro) macro(17)
#define SF_REPEAT_18(macro) SF_REPEAT_17(macro) macro(18)
#define SF_REPEAT_19(macro) SF_REPEAT_18(macro) macro(19)
#define SF_REPEAT_20(macro) SF_REPEAT_19(macro) macro(20)
#define SF_REPEAT_21(macro) SF_REPEAT_20(macro) macro(21)
#define SF_REPEAT_22(macro) SF_REPEAT_21(macro) macro(22)
#define SF_REPEAT_23(macro) SF_REPEAT_22(macro) macro(23)
#define SF_REPEAT_24(macro) SF_REPEAT_23(macro) macro(24)

#define SF_REPEAT_25(macro) SF_REPEAT_24(macro) macro(25)
#define SF_REPEAT_26(macro) SF_REPEAT_25(macro) macro(26)
#define SF_REPEAT_27(macro) SF_REPEAT_26(macro) macro(27)
#define SF_REPEAT_28(macro) SF_REPEAT_27(macro) macro(28)
#define SF_REPEAT_29(macro) SF_REPEAT_28(macro) macro(29)
#define SF_REPEAT_30(macro) SF_REPEAT_29(macro) macro(30)
#define SF_REPEAT_31(macro) SF_REPEAT_30(macro) macro(31)
#define SF_REPEAT_32(macro) SF_REPEAT_31(macro) macro(32)

#define SF_REPEAT_33(macro) SF_REPEAT_32(macro) macro(33)
#define SF_REPEAT_34(macro) SF_REPEAT_33(macro) macro(34)
#define SF_REPEAT_35(macro) SF_REPEAT_34(macro) macro(35)
#define SF_REPEAT_36(macro) SF_REPEAT_35(macro) macro(36)
#define SF_REPEAT_37(macro) SF_REPEAT_36(macro) macro(37)
#define SF_REPEAT_38(macro) SF_REPEAT_37(macro) macro(38)
#define SF_REPEAT_39(macro) SF_REPEAT_38(macro) macro(39)
#define SF_REPEAT_40(macro) SF_REPEAT_39(macro) macro(40)

#define SF_REPEAT_41(macro) SF_REPEAT_40(macro) macro(41)
#define SF_REPEAT_42(macro) SF_REPEAT_41(macro) macro(42)
#define SF_REPEAT_43(macro) SF_REPEAT_42(macro) macro(43)
#define SF_REPEAT_44(macro) SF_REPEAT_43(macro) macro(44)
#define SF_REPEAT_45(macro) SF_REPEAT_44(macro) macro(45)
#define SF_REPEAT_46(macro) SF_REPEAT_45(macro) macro(46)
#define SF_REPEAT_47(macro) SF_REPEAT_46(macro) macro(47)
#define SF_REPEAT_48(macro) SF_REPEAT_47(macro) macro(48)

#define SF_REPEAT_49(macro) SF_REPEAT_48(macro) macro(49)
#define SF_REPEAT_50(macro) SF_REPEAT_49(macro) macro(50)
#define SF_REPEAT_51(macro) SF_REPEAT_50(macro) macro(51)
#define SF_REPEAT_52(macro) SF_REPEAT_51(macro) macro(52)
#define SF_REPEAT_53(macro) SF_REPEAT_52(macro) macro(53)
#define SF_REPEAT_54(macro) SF_REPEAT_53(macro) macro(54)
#define SF_REPEAT_55(macro) SF_REPEAT_54(macro) macro(55)
#define SF_REPEAT_56(macro) SF_REPEAT_55(macro) macro(56)

#define SF_REPEAT_57(macro) SF_REPEAT_56(macro) macro(57)
#define SF_REPEAT_58(macro) SF_REPEAT_57(macro) macro(58)
#define SF_REPEAT_59(macro) SF_REPEAT_58(macro) macro(59)
#define SF_REPEAT_60(macro) SF_REPEAT_59(macro) macro(60)
#define SF_REPEAT_61(macro) SF_REPEAT_60(macro) macro(61)
#define SF_REPEAT_62(macro) SF_REPEAT_61(macro) macro(62)
#define SF_REPEAT_63(macro) SF_REPEAT_62(macro) macro(63)
#define SF_REPEAT_64(macro) SF_REPEAT_63(macro) macro(64)
// and etc.

// generate placeholder sequence _0, _1, _2, ..., _count
#define SF_PLACEHOLDERS(count) SF_CONCAT(SF_PLACEHOLDER_, count)()

#define SF_PLACEHOLDER_1() _0
#define SF_PLACEHOLDER_2() SF_PLACEHOLDER_1(), _1
#define SF_PLACEHOLDER_3() SF_PLACEHOLDER_2(), _2
#define SF_PLACEHOLDER_4() SF_PLACEHOLDER_3(), _3
#define SF_PLACEHOLDER_5() SF_PLACEHOLDER_4(), _4
#define SF_PLACEHOLDER_6() SF_PLACEHOLDER_5(), _5
#define SF_PLACEHOLDER_7() SF_PLACEHOLDER_6(), _6
#define SF_PLACEHOLDER_8() SF_PLACEHOLDER_7(), _7

#define SF_PLACEHOLDER_9() SF_PLACEHOLDER_8(), _8
#define SF_PLACEHOLDER_10() SF_PLACEHOLDER_9(), _9
#define SF_PLACEHOLDER_11() SF_PLACEHOLDER_10(), _10
#define SF_PLACEHOLDER_12() SF_PLACEHOLDER_11(), _11
#define SF_PLACEHOLDER_13() SF_PLACEHOLDER_12(), _12
#define SF_PLACEHOLDER_14() SF_PLACEHOLDER_13(), _13
#define SF_PLACEHOLDER_15() SF_PLACEHOLDER_14(), _14
#define SF_PLACEHOLDER_16() SF_PLACEHOLDER_15(), _15

#define SF_PLACEHOLDER_17() SF_PLACEHOLDER_16(), _16
#define SF_PLACEHOLDER_18() SF_PLACEHOLDER_17(), _17
#define SF_PLACEHOLDER_19() SF_PLACEHOLDER_18(), _18
#define SF_PLACEHOLDER_20() SF_PLACEHOLDER_19(), _19
#define SF_PLACEHOLDER_21() SF_PLACEHOLDER_20(), _20
#define SF_PLACEHOLDER_22() SF_PLACEHOLDER_21(), _21
#define SF_PLACEHOLDER_23() SF_PLACEHOLDER_22(), _22
#define SF_PLACEHOLDER_24() SF_PLACEHOLDER_23(), _23

#define SF_PLACEHOLDER_25() SF_PLACEHOLDER_24(), _24
#define SF_PLACEHOLDER_26() SF_PLACEHOLDER_25(), _25
#define SF_PLACEHOLDER_27() SF_PLACEHOLDER_26(), _26
#define SF_PLACEHOLDER_28() SF_PLACEHOLDER_27(), _27
#define SF_PLACEHOLDER_29() SF_PLACEHOLDER_28(), _28
#define SF_PLACEHOLDER_30() SF_PLACEHOLDER_29(), _29
#define SF_PLACEHOLDER_31() SF_PLACEHOLDER_30(), _30
#define SF_PLACEHOLDER_32() SF_PLACEHOLDER_31(), _31

#define SF_PLACEHOLDER_33() SF_PLACEHOLDER_32(), _32
#define SF_PLACEHOLDER_34() SF_PLACEHOLDER_33(), _33
#define SF_PLACEHOLDER_35() SF_PLACEHOLDER_34(), _34
#define SF_PLACEHOLDER_36() SF_PLACEHOLDER_35(), _35
#define SF_PLACEHOLDER_37() SF_PLACEHOLDER_36(), _36
#define SF_PLACEHOLDER_38() SF_PLACEHOLDER_37(), _37
#define SF_PLACEHOLDER_39() SF_PLACEHOLDER_38(), _38
#define SF_PLACEHOLDER_40() SF_PLACEHOLDER_39(), _39

#define SF_PLACEHOLDER_41() SF_PLACEHOLDER_40(), _40
#define SF_PLACEHOLDER_42() SF_PLACEHOLDER_41(), _41
#define SF_PLACEHOLDER_43() SF_PLACEHOLDER_42(), _42
#define SF_PLACEHOLDER_44() SF_PLACEHOLDER_43(), _43
#define SF_PLACEHOLDER_45() SF_PLACEHOLDER_44(), _44
#define SF_PLACEHOLDER_46() SF_PLACEHOLDER_45(), _45
#define SF_PLACEHOLDER_47() SF_PLACEHOLDER_46(), _46
#define SF_PLACEHOLDER_48() SF_PLACEHOLDER_47(), _47

#define SF_PLACEHOLDER_49() SF_PLACEHOLDER_48(), _48
#define SF_PLACEHOLDER_50() SF_PLACEHOLDER_49(), _49
#define SF_PLACEHOLDER_51() SF_PLACEHOLDER_50(), _50
#define SF_PLACEHOLDER_52() SF_PLACEHOLDER_51(), _51
#define SF_PLACEHOLDER_53() SF_PLACEHOLDER_52(), _52
#define SF_PLACEHOLDER_54() SF_PLACEHOLDER_53(), _53
#define SF_PLACEHOLDER_55() SF_PLACEHOLDER_54(), _54
#define SF_PLACEHOLDER_56() SF_PLACEHOLDER_55(), _55

#define SF_PLACEHOLDER_57() SF_PLACEHOLDER_56(), _56
#define SF_PLACEHOLDER_58() SF_PLACEHOLDER_57(), _57
#define SF_PLACEHOLDER_59() SF_PLACEHOLDER_58(), _58
#define SF_PLACEHOLDER_60() SF_PLACEHOLDER_59(), _59
#define SF_PLACEHOLDER_61() SF_PLACEHOLDER_60(), _60
#define SF_PLACEHOLDER_62() SF_PLACEHOLDER_61(), _61
#define SF_PLACEHOLDER_63() SF_PLACEHOLDER_62(), _62
#define SF_PLACEHOLDER_64() SF_PLACEHOLDER_63(), _63
// and etc.

#define SERIALIZATION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)((template <>), (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType)) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)((), (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType), object)

#define TEMPLATE_SERIALIZATION(mode, object, object_template_header, ...) \
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)(object_template_header, (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType)) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)(object_template_header, (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType), object)

#define CONDITIONAL_SERIALIZATION(mode, object, ...) \
    SF_CONCAT(SF_CONDITIONAL_SERIALIZATION_DECLARATION_, mode)((__VA_ARGS__), (template <class ArchiveType>), (ArchiveType)) \
    SF_CONCAT(SF_CONDITIONAL_SERIALIZATION_DEFINITION_, mode)((__VA_ARGS__), (template <class ArchiveType>), (ArchiveType), object)

// allow to hide implementation to translation unit, and declare interface in header
#define SERIALIZATION_DECLARATION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)((template <>), (__VA_ARGS__), (), (::sf::core::ioarchive_t))

#define SERIALIZATION_DEFINITION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)((), (__VA_ARGS__), (), (::sf::core::ioarchive_t), object)

// Impl
#define SF_SERIALIZATION_DECLARATION(mode, object_template_header, object_type, archive_template_header, archive_type) \
    SF_DEPAREN(object_template_header) \
    struct xxsf_##mode<SF_DEPAREN(object_type)> { \
        SF_DEPAREN(archive_template_header) \
        xxsf_##mode(SF_DEPAREN(archive_type)&, SF_DEPAREN(object_type)&); \
    };

#define SF_SERIALIZATION_DECLARATION_save(object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(save, object_template_header, object_type, archive_template_header, archive_type)

#define SF_SERIALIZATION_DECLARATION_load(object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(load, object_template_header, object_type, archive_template_header, archive_type)

#define SF_SERIALIZATION_DECLARATION_saveload(object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(save, object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(load, object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(saveload, object_template_header, object_type, archive_template_header, archive_type)

#define SF_SERIALIZATION_DEFINITION(mode, object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_DEPAREN(object_template_header) \
    SF_DEPAREN(archive_template_header) \
    xxsf_##mode<SF_DEPAREN(object_type)>::xxsf_##mode(SF_DEPAREN(archive_type)& archive, SF_DEPAREN(object_type)& object)

#define SF_SERIALIZATION_DEFINITION_save(object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_SERIALIZATION_DEFINITION(save, object_template_header, object_type, archive_template_header, archive_type, object)

#define SF_SERIALIZATION_DEFINITION_load(object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_SERIALIZATION_DEFINITION(load, object_template_header, object_type, archive_template_header, archive_type, object)

#define SF_SERIALIZATION_DEFINITION_saveload(object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_SERIALIZATION_DEFINITION(save, object_template_header, object_type, archive_template_header, archive_type, object) \
    { xxsf_saveload<SF_DEPAREN(object_type)>(archive, object); } \
    SF_SERIALIZATION_DEFINITION(load, object_template_header, object_type, archive_template_header, archive_type, object) \
    { xxsf_saveload<SF_DEPAREN(object_type)>(archive, object); }\
    SF_SERIALIZATION_DEFINITION(saveload, object_template_header, object_type, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION(mode, object_type_condition, archive_template_header, archive_type) \
    template <typename T> \
    struct xxsf_##mode<T, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type> { \
        SF_DEPAREN(archive_template_header) \
        xxsf_##mode(SF_DEPAREN(archive_type)&, T&); \
    };

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION_save(object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(save, object_type_condition, archive_template_header, archive_type)

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION_load(object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(load, object_type_condition, archive_template_header, archive_type)

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION_saveload(object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(save, object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(load, object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(saveload, object_type_condition, archive_template_header, archive_type)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION(mode, object_type_condition, archive_template_header, archive_type, object) \
    template <typename T> \
    SF_DEPAREN(archive_template_header) \
    xxsf_##mode<T, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type>::xxsf_##mode(SF_DEPAREN(archive_type)& archive, T& object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_save(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_load(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_saveload(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<T>(archive, object); } \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<T>(archive, object); }\
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(saveload, object_type_condition, archive_template_header, archive_type, object)
// ~Impl

// should be in global namespace
template <class T, typename enable = void> struct xxsf_save;
template <class T, typename enable = void> struct xxsf_load;
template <class T, typename enable = void> struct xxsf_saveload;

class xxsf
{
public:
    template <class T, typename = void> struct is_has_static_traits : std::false_type {};
    template <class T>
    struct is_has_static_traits<T, ::sf::meta::void_t<decltype(&T::xxstatic_traits)>> : std::true_type {};

    template <class T, typename = void> struct is_has_traits : std::false_type {};
    template <class T>
    struct is_has_traits<T, ::sf::meta::void_t<decltype(&T::xxtrait)>> : std::true_type {};

public:
    template <class T> struct is_has_inner_traits
    {
        static constexpr bool value = is_has_static_traits<T>::value and is_has_traits<T>::value; // delay access
    };

public:
    template <class Base, class ArchiveType, class Derived>
    static void serialize_base(ArchiveType& archive, Derived& object)
    {
        archive & static_cast<Base&>(object);
    }

public:
    template <class T, SF_REQUIRE(not is_has_inner_traits<T>::value)>
    static ::xxsf_instantiable_traits_key_type traits(T& object)
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SF_REQUIRE(is_has_inner_traits<T>::value)>
    static ::xxsf_instantiable_traits_key_type traits(T& object) noexcept
    {
        return object.xxtrait();
    }

    template <class T, SF_REQUIRE(not is_has_inner_traits<T>::value)>
    static ::xxsf_instantiable_traits_key_type static_traits() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SF_REQUIRE(is_has_inner_traits<T>::value)>
    static constexpr ::xxsf_instantiable_traits_key_type static_traits() noexcept
    {
        return T::xxstatic_traits();
    }

    template <class T, SF_REQUIRE(not is_has_inner_traits<T>::value)>
    static ::xxsf_instantiable_traits_key_type traits() noexcept
    {
        static_assert(::xxsf_instantiable_traits<T>::key == ::xxsf_instantiable_traits_base_key,
            "Export instantiable traits is not allowed using typeid.");

        return static_traits<T>();
    }

    template <class T, SF_REQUIRE(is_has_inner_traits<T>::value)>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr ::xxsf_instantiable_traits_key_type traits() noexcept
#else
    static ::xxsf_instantiable_traits_key_type traits() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto traits_key = ::xxsf_instantiable_traits<T>::key;

        return traits_key == ::xxsf_instantiable_traits_base_key
             ? static_traits<T>()
             : traits_key;
    }
};

#include <unordered_map> // unordered_map

// You can include given file to other dir level

namespace sf
{

namespace memory
{

using shared_t = meta::shared_common_t;
using raw_t = meta::raw_common_t;

} // namespace memory

namespace meta
{

template <typename T> struct is_memory_shared : std::is_same<T, memory::shared_t> {};
template <typename T> struct is_memory_raw : std::is_same<T, memory::raw_t> {};

template <typename T> struct is_memory : one<is_memory_shared<T>, is_memory_raw<T>> {};

} // namespace meta

namespace memory
{

template <typename T>
using shared_ptr = std::shared_ptr<T>;

template <typename T>
using raw_ptr = T*;

template <typename T>
struct ptr_traits
{
    using item = std::nullptr_t;
};

template <typename T>
struct ptr_traits<std::shared_ptr<T>>
{
    using traits    = shared_t;

    template <typename U>
    using wrapper  = shared_ptr<U>;

    using type     = std::shared_ptr<T>;
    using item     = T;

    using void_ptr = std::shared_ptr<void>;
};

template <typename T>
struct ptr_traits<T*>
{
    using traits    = raw_t;

    template <typename U>
    using wrapper  = raw_ptr<U>;

    using type     = T*;
    using item     = T;

    using void_ptr = void*;
};

template <typename T>
struct ptr_traits<std::weak_ptr<T>>
{
    using type     = std::weak_ptr<T>;
    using item     = T;
};

template <typename T>
struct ptr_traits<std::unique_ptr<T>>
{
    using type     = std::unique_ptr<T>;
    using item     = T;
};

template <typename T>
using void_ptr = typename ptr_traits<T>::void_ptr;

template <typename T>
struct factory_t
{
    static std::shared_ptr<T> shared()
    {
        return std::make_shared<T>();
    }

    static T* raw()
    {
        return new T{};
    }
};

template <typename To, typename T,
          SF_REQUIRE(meta::is_raw_pointer<T>::value)>
raw_ptr<To> dynamic_pointer_cast(const T& pointer)
{
    return dynamic_cast<raw_ptr<To>>(pointer);
}

template <typename To, typename T,
          SF_REQUIRE(meta::is_shared_pointer<T>::value)>
shared_ptr<To> dynamic_pointer_cast(const T& pointer)
{
    auto address = memory::dynamic_pointer_cast<To>(pointer.get());
    return address == nullptr ? shared_ptr<To>() : shared_ptr<To>(pointer, address);
}

template <typename To, typename T,
          typename Traits = ptr_traits<T>,
          SF_REQUIRE(meta::one<meta::is_null_pointer<T>,
                               meta::all<meta::is_pointer<T>,
                                         meta::negation<meta::is_static_castable<typename Traits::item*, To*>>>>::value)>
typename Traits::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
{
    return nullptr;
}

template <typename To, typename T,
          SF_REQUIRE(meta::all<meta::is_raw_pointer<T>,
                               meta::is_static_castable<typename ptr_traits<T>::item*, To*>>::value)>
raw_ptr<To> static_pointer_cast(const T& pointer) noexcept
{
    return static_cast<raw_ptr<To>>(pointer);
}

template <typename To, typename From, typename T,
          typename Traits = ptr_traits<T>,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::negation<meta::is_static_castable<From*, To*>>>::value)>
typename Traits::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
{
    return nullptr;
}

template <typename To, typename T,
          SF_REQUIRE(meta::all<meta::is_shared_pointer<T>,
                               meta::is_static_castable<typename ptr_traits<T>::item*, To*>>::value)>
shared_ptr<To> static_pointer_cast(const T& pointer) noexcept
{
    auto address = memory::static_pointer_cast<To>(pointer.get());
    return shared_ptr<To>(pointer, address);
}

template <typename To, typename From, typename T,
          typename Traits = ptr_traits<T>,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::is_static_castable<typename Traits::item*, From*>,
                               meta::is_static_castable<From*, To*>>::value)>
typename Traits::template wrapper<To> static_pointer_cast(const T& pointer) noexcept
{
    return memory::static_pointer_cast<To>(memory::static_pointer_cast<From>(pointer));
}

template <typename T,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void_ptr<T> pure(const T& pointer) noexcept
{
    return memory::static_pointer_cast<void>(pointer);
}

template <typename T,
          SF_REQUIRE(meta::all<meta::is_pointer<T>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void_ptr<T> pure(const T& pointer_to_polymorphic)
{
    return memory::dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

inline raw_ptr<void> pure(std::nullptr_t pointer) noexcept { return nullptr; }

template <typename dT, typename T,
          SF_REQUIRE(meta::is_pointer<T>::value)>
void assign(T& pointer, const void_ptr<T>& address) noexcept
{
    pointer = memory::static_pointer_cast<dT>(address);
}

template <typename To, typename From = To, typename TraitsType,
          SF_REQUIRE(meta::all<meta::is_memory<TraitsType>,
                               meta::one<meta::negation<meta::is_static_castable<From*, To*>>,
                                         std::is_abstract<From>>>::value)>
std::nullptr_t allocate() noexcept
{
    return nullptr;
}

template <typename To, typename From = To, typename TraitsType,
          SF_REQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                               meta::is_memory_shared<TraitsType>,
                               meta::is_static_castable<From*, To*>>::value)>
shared_ptr<To> allocate()
{
    auto instance = factory_t<From>::shared();
    return memory::static_pointer_cast<To>(instance);
}

template <typename To, typename From = To, typename TraitsType,
          SF_REQUIRE(meta::all<meta::negation<std::is_abstract<From>>,
                               meta::is_memory_raw<TraitsType>,
                               meta::is_static_castable<From*, To*>>::value)>
raw_ptr<To> allocate()
{
    auto instance = factory_t<From>::raw();
    return memory::static_pointer_cast<To>(instance);
}

template <typename To, typename From = To>
shared_ptr<To> allocate_shared()
{
    return memory::allocate<To, From, shared_t>();
}

template <typename To, typename From = To>
raw_ptr<To> allocate_raw()
{
    return memory::allocate<To, From, raw_t>();
}

template <typename To, typename From = To, typename T,
          SF_REQUIRE(meta::is_pointer<T>::value)>
void allocate(T& pointer)
{
    pointer = memory::allocate<To, From, typename ptr_traits<T>::traits>();
}

template <typename T, typename dT = typename ptr_traits<T>::item,
          SF_REQUIRE(meta::is_raw_pointer<T>::value)>
raw_ptr<dT> raw(const T& pointer) { return pointer; }

template <typename T, typename dT = typename ptr_traits<T>::item,
          SF_REQUIRE(meta::is_shared_pointer<T>::value)>
raw_ptr<dT> raw(const T& pointer) { return pointer.get(); }

template <typename ByteType = char, typename T>
const ByteType* const_byte_cast(T* data) noexcept
{
    return reinterpret_cast<const ByteType*>(data);
}

template <typename ByteType = char, typename T>
ByteType* byte_cast(T* data) noexcept
{
    return reinterpret_cast<ByteType*>(data);
}

} // namespace memory

} // namespace sf

#ifdef SF_DEBUG
#endif // SD_DEBUG

namespace sf
{

namespace core
{

class polymorphic_archive_t
{
public:
    template <class T> static void save(ioarchive_t& archive, T& data)
    {
        call<::xxsf_oarchive_registry>(archive, data);
    }

    template <class T> static void load(ioarchive_t& archive, T& data)
    {
        call<::xxsf_iarchive_registry>(archive, data);
    }

private:
    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKey,
              class T, SF_REQUIRE(ArchiveKey == ::xxsf_archive_traits_base_key)>
    static void call(ioarchive_t& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKey = 0, class T,
              SF_REQUIRE(ArchiveKey < ::xxsf_archive_traits_base_key)>
    static void call(core::ioarchive_t& archive, T& data)
    {
        using DerivedArchive = typename ArchiveRegistryTemplate<ArchiveKey>::type;

        if (::xxsf_archive_traits<DerivedArchive>::key == archive.trait)
            return try_call<DerivedArchive>(archive, data);

        call<ArchiveRegistryTemplate, ArchiveKey + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(::xxsf_archive_traits<DerivedArchive>::key == ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(::xxsf_archive_traits<DerivedArchive>::key != ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t& archive, T& data)
    {
    #ifdef SF_DEBUG
        if (typeid(archive) != typeid(DerivedArchive))
            throw "The read/write archive was registered incorrect.";
    #endif // SF_DEBUG
        try_call_impl<DerivedArchive>(static_cast<DerivedArchive&>(archive), data);
    }

    template <class DerivedArchive, class T>
    static void try_call_impl(iarchive_common_t& archive, T& data)
    {
        ::xxsf_load<T>(static_cast<DerivedArchive&>(archive), data);
    }
    template <class DerivedArchive, class T>
    static void try_call_impl(oarchive_common_t& archive, T& data)
    {
        ::xxsf_save<T>(static_cast<DerivedArchive&>(archive), data);
    }
};

template <typename T,
          SF_REQUIRE(meta::is_unsupported<T>::value)>
ioarchive_t& operator& (ioarchive_t& archive, T const& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'sf::ioarchive_t'.");

    return archive;
}

template <typename T,
          SF_REQUIRE(not meta::is_unsupported<T>::value)>
ioarchive_t& operator<< (ioarchive_t& archive, T const& data)
{
    polymorphic_archive_t::save(archive, const_cast<T&>(data));
    return archive;
}

template <typename T,
          SF_REQUIRE(not meta::is_unsupported<T>::value)>
ioarchive_t& operator>> (ioarchive_t& archive, T const& data)
{
    polymorphic_archive_t::load(archive, const_cast<T&>(data));
    return archive;
}

template <typename T,
          SF_REQUIRE(not meta::is_unsupported<T>::value)>
ioarchive_t& operator& (ioarchive_t& archive, T const& data)
{
    return archive.readonly ? archive >> data : archive << data;
}

} // namespace core

} // namespace sf

// By default library will use instantiable_t type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
// Note that: any defined INSTANTIABLE_TYPE must have virtual destructor
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::sf::instantiable_t
#endif // INSTANTIABLE_TYPE

namespace sf
{

struct instantiable_t { virtual ~instantiable_t() = default; };

} // namespace sf

// Auto instantiable type registration
#define SERIALIZATION_FIXTURE(...)                                                                      \
    ::sf::dynamic::instantiable_fixture_t<__VA_ARGS__> xxfixture;

#define SERIALIZATION_TRAITS(...)                                                                       \
    static constexpr ::xxsf_instantiable_traits_key_type xxstatic_traits() noexcept {                   \
        return SF_STATIC_HASH(#__VA_ARGS__);                                                            \
    }                                                                                                   \
    virtual ::xxsf_instantiable_traits_key_type xxtrait() const noexcept {                              \
        return ::xxsf::traits<__VA_ARGS__>();                                                           \
    }

namespace sf
{

namespace dynamic
{

class instantiable_registry_t
{
public:
    using instantiable_type = INSTANTIABLE_TYPE;

private:
    template <typename ItemType>
    using InnerTable = std::unordered_map<::xxsf_instantiable_traits_key_type, ItemType>;

private:
    struct instantiable_proxy_t
    {
        std::shared_ptr<instantiable_type>(*shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*cast_shared)(std::shared_ptr<void>) = nullptr;

        instantiable_type*(*raw)() = nullptr;
        instantiable_type*(*cast_raw)(void*) = nullptr;

        void(*save)(core::ioarchive_t&, instantiable_type*) = nullptr;
        void(*load)(core::ioarchive_t&, instantiable_type*) = nullptr;
    };

private:
    InnerTable<instantiable_proxy_t> registry_;

private:

    instantiable_registry_t() : registry_() {}
    instantiable_registry_t(const instantiable_registry_t&) = delete;
    instantiable_registry_t& operator=(const instantiable_registry_t&) = delete;

public:
    static instantiable_registry_t& instance() noexcept
    {
        static instantiable_registry_t self;
        return self;
    }

public:
    // we should use external check
    template <typename dT> struct is_instantiable : meta::is_cast_allowed<dT*, instantiable_type*> {};

public:
    template <class T, SF_REQUIRE(not is_instantiable<T>::value)>
    void update(::xxsf_instantiable_traits_key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'instantiable_t'.";
    }

    template <class T, SF_REQUIRE(is_instantiable<T>::value)>
    void update(::xxsf_instantiable_traits_key_type key)
    {
        if (is_registered(key)) return;

        instantiable_proxy_t proxy;

        proxy.shared = [] { return memory::allocate_shared<instantiable_type, T>(); };

        proxy.cast_shared = [](std::shared_ptr<void> address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.raw = [] { return memory::allocate_raw<instantiable_type, T>(); };

        proxy.cast_raw = [](void* address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.save = [](core::ioarchive_t& archive, instantiable_type* instance)
        {
            archive << *memory::dynamic_pointer_cast<T>(instance);
        };

        proxy.load = [](core::ioarchive_t& archive, instantiable_type* instance)
        {
            archive >> *memory::dynamic_pointer_cast<T>(instance);
        };

        registry_.emplace(key, proxy);
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_shared<TraitsType>::value)>
    std::shared_ptr<instantiable_type> clone(::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).shared();
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_raw<TraitsType>::value)>
    instantiable_type* clone(::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> address, ::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).cast_shared(address);
    }

    instantiable_type* cast(void* address, ::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).cast_raw(address);
    }

    template <typename Pointer/*,
              SF_REQUIRE(meta::is_has_any_save_mode<typename meta::dereference<Pointer>::type>::value)*/>
    void save(core::ioarchive_t& archive, Pointer& pointer)
    {
        const auto key = ::xxsf::traits(*pointer);
        registry(key).save(archive, pointer);
    }

    template <typename Pointer/*,
              SF_REQUIRE(meta::is_has_any_load_mode<typename meta::dereference<Pointer>::type>::value)*/>
    void load(core::ioarchive_t& archive, Pointer& pointer)
    {
        const auto key = ::xxsf::traits(*pointer);
        registry(key).load(archive, pointer);
    }

public:
    bool is_registered(::xxsf_instantiable_traits_key_type key)
    {
        return registry_.find(key) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    instantiable_proxy_t& registry(::xxsf_instantiable_traits_key_type key)
    {
        // It happens if the class with the given key has not beed public inherited
        // from the instantiable class or not registered with fixture object.
        auto it = registry_.find(key);
        if (it == registry_.end())
            throw "The 'sf::dynamic::instantiable_registry_t' does not has instance with input key.";

        return it->second;
    }
};

template <class T>
class instantiable_fixture_t
{
private:
    static bool lock_;

public:
    instantiable_fixture_t() { call<T>(); }

public:
    template <typename dT = T,
              SF_REQUIRE(instantiable_registry_t::is_instantiable<dT>::value)>
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = instantiable_registry_t::instance();

        const auto key = ::xxsf::template traits<T>();
    #ifdef SF_DEBUG
        if (registry.is_registered(key))
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with unique key.";
    #endif // SF_DEBUG

        registry.update<T>(key);
    }

    template <typename dT = T,
              SF_REQUIRE(not instantiable_registry_t::is_instantiable<dT>::value)>
    static void call() noexcept { /*pass*/ }
};

template <class T>
bool instantiable_fixture_t<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

namespace sf
{

namespace dynamic
{

class any_registry_t
{
private:
    struct any_proxy_t
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*save)(core::ioarchive_t&, std::any&) = nullptr;
        void(*load)(core::ioarchive_t&, std::any&) = nullptr;
    };

private:
    using InnerTable = std::unordered_map<let::u64, any_proxy_t>;

private:
    InnerTable registry_;

private:
    any_registry_t() : registry_() {}

    any_registry_t(const any_registry_t&) = delete;
    any_registry_t& operator=(const any_registry_t&) = delete;

public:
    static any_registry_t& instance() noexcept
    {
        static any_registry_t self;
        return self;
    }

    template <typename T> void update(let::u64 hash)
    {
        if (is_registered(hash)) return;

        any_proxy_t proxy;

        proxy.save = [](core::ioarchive_t& archive, std::any& any)
        {
            archive << std::any_cast<T&>(any);
        };

        proxy.load = [](core::ioarchive_t& archive, std::any& any)
        {
            any.emplace<T>();
            archive >> std::any_cast<T&>(any);
        };

        registry_.emplace(hash, proxy);
    }

public:
    void save(core::ioarchive_t& archive, std::any& any, let::u64 hash)
    {
        registry(hash).save(archive, any);
    }

    void load(core::ioarchive_t& archive, std::any& any, let::u64 hash)
    {
        registry(hash).load(archive, any);
    }

public:
    bool is_registered(let::u64 hash)
    {
        return registry_.find(hash) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    const any_proxy_t& registry(let::u64 hash)
    {
        // It happens if the type not registered with fixture object.
        auto it = registry_.find(hash);
        if (it == registry_.end())
            throw "The 'sf::any_registry_t' must registry type with specify hash code.";

        return it->second;
    }
};

template <typename T>
class any_fixture_t
{
private:
    static bool lock_;

public:
    any_fixture_t() { call(); }

public:
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = any_registry_t::instance();

        auto hash = SF_TYPE_HASH(typeid(T));
    #ifdef SF_DEBUG
        if (registry.is_registered(hash))
            throw "The 'sf::dynamic::any_registry_t' must contains unique hashes.";
    #endif // SF_DEBUG

        registry.update<T>(hash);
    }
};

template <typename T>
bool any_fixture_t<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#endif // if

namespace sf
{

namespace dynamic
{

class extern_registry_t
{
public:
    template <class ArchiveType, typename T,
              SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static ::xxsf_instantiable_traits_key_type save_key(ArchiveType& archive, T& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto key = ::xxsf::traits(*pointer);
        archive & key;

        return key;
    }

    template <class ArchiveType, typename T,
              SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static ::xxsf_instantiable_traits_key_type load_key(ArchiveType& archive, T& pointer)
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        ::xxsf_instantiable_traits_key_type key{};
        archive & key;

        return key;
    }

private:
    template <typename T> struct is_pointer_to_instantiable
        : meta::all<instantiable_registry_t::is_instantiable<typename meta::dereference<T>::type>,
                    meta::is_pointer_to_polymorphic<T>> {};

public:
    template <typename T, SF_REQUIRE(is_pointer_to_instantiable<T>::value)>
    static void save(core::ioarchive_t& archive, T& pointer, ::xxsf_instantiable_traits_key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = memory::raw(pointer);
        instantiable_registry_t::instance().save(archive, raw_pointer);
    }

    template <typename T, SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void load(core::ioarchive_t& archive, T& pointer, ::xxsf_instantiable_traits_key_type key, memory::void_ptr<T>& cache)
    {
        using TraitsType = typename memory::ptr_traits<T>::traits;
        using dT = typename meta::dereference<T>::type;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto& registry = instantiable_registry_t::instance();

        auto cloned = registry.clone<TraitsType>(key);

        pointer = memory::dynamic_pointer_cast<dT>(cloned);
        cache = memory::pure(pointer);

        auto raw_pointer = memory::raw(pointer);
        registry.load(archive, raw_pointer);
    }

    template <typename T, SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void assign(T& pointer, const memory::void_ptr<T>& address, ::xxsf_instantiable_traits_key_type key)
    {
        using dT = typename meta::dereference<T>::type;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto casted = instantiable_registry_t::instance().cast(address, key);
        pointer = memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace sf

#include <utility> // forward

namespace sf
{

namespace tracking
{

using shared_t = meta::shared_common_t;
using raw_t = meta::raw_common_t;

template <typename T>
struct track_traits;

template <typename T>
struct track_traits<memory::shared_ptr<T>>
{
    using type = tracking::shared_t;
};

template <typename T>
struct track_traits<memory::raw_ptr<T>>
{
    using type = tracking::raw_t;
};

template <typename T>
struct reverse_traits;

template <>
struct reverse_traits<shared_t>
{
    using type = raw_t;
};

template <>
struct reverse_traits<raw_t>
{
    using type = shared_t;
};

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_shared : std::is_same<T, tracking::shared_t> {};
template <typename T> struct is_track_raw : std::is_same<T, tracking::raw_t> {};

} // namespace meta

} // namespace sf

namespace sf
{

namespace detail
{

template <typename HashType = let::u64>
struct pair_hash_t
{
    template <typename T1, typename T2>
    HashType operator() (const std::pair<T1, T2>& pair) const noexcept
    {
        HashType seed{};

        detail::hash_combine(seed, pair.first);
        detail::hash_combine(seed, pair.second);

        return seed;
    }
};

} // namespace detail

namespace tracking
{

struct hierarchy_t {};

template <typename KeyType, typename TraitsType = ::xxsf_instantiable_traits_key_type>
using hierarchy_track_t = std::unordered_map<std::pair<KeyType, TraitsType>, bool, detail::pair_hash_t<TraitsType>>;

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_hierarchy : std::is_same<T, tracking::hierarchy_t> {};

} // namespace meta

} // namespace sf

#include <vector> // vector
#include <fstream> // ifstream, ofstream

#ifndef SF_BYTE_STREAM_RESERVE_SIZE
    #define SF_BYTE_STREAM_RESERVE_SIZE std::size_t(4096)
#endif // SF_BYTE_STREAM_RESERVE_SIZE

namespace sf
{

namespace wrapper
{

template <typename OutStream = std::vector<unsigned char>>
class obyte_stream_t
{
protected:
    using item_type = typename OutStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::obyte_stream_t'.");

public:
    OutStream& storage;

public:
    obyte_stream_t(OutStream& stream) noexcept : storage(stream)
    {
        storage.reserve(SF_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = memory::const_byte_cast<item_type>(data);
        while (size > 0)
        {
            storage.emplace_back(*it++);
            --size;
        }
    }
};

template <typename InStream = std::vector<unsigned char>>
struct ibyte_stream_t
{
protected:
    using item_type = typename InStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::ibyte_stream_t'.");

public:
    InStream& storage;
    std::size_t offset;

    ibyte_stream_t(InStream& stream) noexcept : storage(stream), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = memory::byte_cast<item_type>(data);
        while (size > 0)
        {
            *it++ = storage[offset++];
            --size;
        }
    }
};

template <typename OutStream = std::ofstream>
class ofile_stream_t
{
public:
    OutStream& file;

public:
    ofile_stream_t(OutStream& stream) noexcept : file(stream) {}

    template <typename T>
    void call(const T* data, std::size_t memory_size)
    {
        file.write(memory::const_byte_cast(data), memory_size);
    }
};

template <typename InStream = std::ifstream>
class ifile_stream_t
{
public:
    InStream& file;

public:
    ifile_stream_t(InStream& stream) noexcept : file(stream) {}

    template <typename T>
    void call(T* data, std::size_t memory_size)
    {
        file.read(memory::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace sf

namespace sf
{

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t>
class oarchive_t : public core::ioarchive_t, public core::oarchive_common_t
{
public:
    using TrackingKeyType = std::uintptr_t;
    using TrackingTable = std::unordered_map<TrackingKeyType, bool>;
    using HierarchyTrackingTable = tracking::hierarchy_track_t<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable track_shared_;
    TrackingTable track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    template <typename OutStream> oarchive_t(OutStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_shared_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_raw_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator<< (T const& data) -> oarchive_t&;

    template <typename T>
    auto operator& (T const& data) -> oarchive_t&;

    template <typename T, typename... Tn>
    auto operator() (T const& data, Tn const&... data_n) -> oarchive_t&;

    auto operator() () noexcept -> oarchive_t& { return *this; }
};

// create default oarchive_t with wrapper::obyte_stream_t<>
template <typename OutStream>
oarchive_t<wrapper::obyte_stream_t<OutStream>> oarchive(OutStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename OutStream>
oarchive_t<StreamWrapper<OutStream>, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename OutStream>
oarchive_t<StreamWrapper, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper, class Registry>
template <typename OutStream>
oarchive_t<StreamWrapper, Registry>::oarchive_t(OutStream& stream)
    : core::ioarchive_t(::xxsf_archive_traits<oarchive_t>::key, false)
    , archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class StreamWrapper, class Registry>
template <typename T>
auto oarchive_t<StreamWrapper, Registry>::operator<< (T const& data) -> oarchive_t&
{
    return operator()(data);
}

template <class StreamWrapper, class Registry>
template <typename T>
auto oarchive_t<StreamWrapper, Registry>::operator& (T const& data) -> oarchive_t&
{
    return operator()(data);
}

template <class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto oarchive_t<StreamWrapper, Registry>::operator() (T const& data, Tn const&... data_n) -> oarchive_t&
{
    ::xxsf_save<T>(*this, const_cast<T&>(data));
    return operator()(data_n...);
}

} // namespace sf

namespace sf
{

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t>
class iarchive_t : public core::ioarchive_t, public core::iarchive_common_t
{
private:
    template <typename VoidPointer>
    struct track_data_t { VoidPointer address = nullptr; };

public:
    using shared_t = track_data_t<memory::shared_ptr<void>>;
    using raw_t = track_data_t<memory::raw_ptr<void>>;

public:
    using TrackingKeyType = std::uintptr_t;

    template <typename TrackData>
    using TrackingTable = std::unordered_map<TrackingKeyType, TrackData>;

    using HierarchyTrackingTable = tracking::hierarchy_track_t<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable<shared_t> track_shared_;
    TrackingTable<raw_t> track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    template <typename InStream> iarchive_t(InStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable<shared_t>& { return track_shared_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable<raw_t>& { return track_raw_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator>> (T const& data) -> iarchive_t&;

    template <typename T>
    auto operator& (T const& data) -> iarchive_t&;

    template <typename T, typename... Tn>
    auto operator() (T const& data, Tn const&... data_n) -> iarchive_t&;

    auto operator() () -> iarchive_t& { return *this; }
};

// create default iarchive_t with wrapper::ibyte_stream_t<>
template <typename InStream>
iarchive_t<wrapper::ibyte_stream_t<InStream>> iarchive(InStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename InStream>
iarchive_t<StreamWrapper<InStream>, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename InStream>
iarchive_t<StreamWrapper, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper, class Registry>
template <typename InStream>
iarchive_t<StreamWrapper, Registry>::iarchive_t(InStream& stream)
    : core::ioarchive_t(::xxsf_archive_traits<iarchive_t>::key, true)
    , archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class StreamWrapper, class Registry>
template <typename T>
auto iarchive_t<StreamWrapper, Registry>::operator>> (T const& data) -> iarchive_t&
{
    return operator()(data);
}

template <class StreamWrapper, class Registry>
template <typename T>
auto iarchive_t<StreamWrapper, Registry>::operator& (T const& data) -> iarchive_t&
{
    return operator()(data);
}

template <class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto iarchive_t<StreamWrapper, Registry>::operator() (T const& data, Tn const&... data_n) -> iarchive_t&
{
    ::xxsf_load<T>(*this, const_cast<T&>(data));
    return operator()(data_n...);
}

} // namespace sf

namespace sf
{

namespace apply
{

struct apply_functor_t {};

} // namespace apply

namespace meta
{

template <typename T> struct is_apply_functor : std::is_base_of<apply::apply_functor_t, T> {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, apply_functor, ::sf::meta::is_apply_functor<typename std::decay<T>::type>::value)
{
    apply_functor(archive);
}

namespace sf
{

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::is_ioarchive<ArchiveType>::value)>
void binary(ArchiveType& archive, T& data)
{
    archive.stream().call(std::addressof(data), sizeof(T));
}

namespace apply
{

template <typename T>
struct binary_functor_t : apply_functor_t
{
    T& data;

    binary_functor_t(T& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { binary(archive, data); }
};

} // namespace apply

template <typename T> apply::binary_functor_t<T> binary(T& object) noexcept { return { object }; }

} // namespace sf

namespace sf
{

namespace detail
{

template <class ArchiveType, typename T, typename KeyType,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_save(ArchiveType& archive, T& pointer, KeyType track_key) noexcept { /*pass*/ }

template <class ArchiveType, typename T, typename KeyType,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void native_save(ArchiveType& archive, T& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_load(ArchiveType& archive, T& pointer, memory::void_ptr<T>& address) noexcept
{
    memory::assign<typename meta::dereference<T>::type>(pointer, address);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void native_load(ArchiveType& archive, T& pointer, memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

namespace sf
{

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_pointer_to_standard_layout<T>>::value)>
void strict(ArchiveType& archive, T& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_pointer_to_standard_layout<T>>::value)>
void strict(ArchiveType& archive, T& pointer, memory::void_ptr<T>& cache)
{
    using item_type = typename memory::ptr_traits<T>::item;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    memory::allocate<item_type>(pointer);
    cache = memory::pure(pointer);

    archive & (*pointer);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void strict(ArchiveType& archive, T& pointer)
{
    auto& registry = archive.registry();

    const auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void strict(ArchiveType& archive, T& pointer, memory::void_ptr<T>& cache)
{
    auto& registry = archive.registry();

    const auto id = registry.load_key(archive, pointer);
    registry.load(archive, pointer, id, cache);
}

// verison without cache using
template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_serializable_pointer<T>>::value)>
void strict(ArchiveType& archive, T& pointer)
{
    memory::void_ptr<T> cache = nullptr;
    strict(archive, pointer, cache);
}

namespace detail
{

template <class ArchiveType, typename T,
          typename KeyType = typename ArchiveType::TrackingKeyType,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(ArchiveType& archive, T& pointer)
{
    auto pure = memory::pure(pointer);
    auto key = reinterpret_cast<KeyType>(memory::raw(pure));

    archive & key;
    return key;
}

template <class ArchiveType, typename T,
          typename KeyType = typename ArchiveType::TrackingKeyType,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_serializable_pointer<T>>::value)>
KeyType refer_key(ArchiveType& archive, T& pointer)
{
#ifdef SF_DEBUG
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_DEBUG

    KeyType key{};
    archive & key;

    return key;
}

} // namespace detail

namespace apply
{

template <typename T>
struct strict_functor_t : public apply_functor_t
{
    T& data;

    strict_functor_t(T& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { strict(archive, data); }
};

} // namespace apply

template <typename T>
apply::strict_functor_t<T> strict(T& parameter) noexcept { return { parameter }; }

} // namespace sf

namespace sf
{

namespace tracking
{

template <typename TrackType, class ArchiveType, typename KeyType,
          SF_REQUIRE(meta::is_ioarchive<ArchiveType>::value)>
bool is_track(ArchiveType& archive, KeyType key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class ArchiveType, typename KeyType,
          SF_REQUIRE(meta::is_ioarchive<ArchiveType>::value)>
bool is_mixed(ArchiveType& archive, KeyType key)
{
    using reverse_track_type = typename reverse_traits<TrackType>::type;
    return is_track<reverse_track_type>(archive, key);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_pointer<T>>::value)>
void track(ArchiveType& archive, T& pointer)
{
    using track_type = typename tracking::track_traits<T>::type;

    const auto key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

#ifdef SF_DEBUG
    if (is_mixed<track_type>(archive, key))
        throw "Mixed pointer tracking is not allowed.";
#endif // SF_DEBUG

    auto& is_tracking = archive.template tracking<track_type>()[key];

    if (not is_tracking)
    {
        is_tracking = true;
        strict(archive, pointer); // call the strict serialization of not tracking pointer
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::negation<meta::is_pointer<T>>>::value)>
void track(ArchiveType& archive, T& data)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    auto address = memory::pure(std::addressof(data));
    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.template tracking<tracking::raw_t>()[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_pointer<T>>::value)>
void track(ArchiveType& archive, T& pointer)
{
    using track_type = typename tracking::track_traits<T>::type;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    const auto key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

    auto& item = archive.template tracking<track_type>()[key];

    if (item.address == nullptr)
    {
        // call the strict serialization of not tracking pointer
        strict(archive, pointer, item.address);
    }
    else
    {
        detail::native_load(archive, pointer, item.address);
    }
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::negation<meta::is_pointer<T>>>::value)>
void track(ArchiveType& archive, T& data)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<tracking::raw_t>()[key];

    if (item.address != nullptr)
        throw  "The read tracking data is already tracked.";

    item.address = memory::pure(std::addressof(data));

    archive & data;
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<T>,
                               meta::is_serializable_raw_pointer<T>>::value)>
void raw(ArchiveType& archive, T& pointer)
{
    if (detail::refer_key(archive, pointer)) // serialize refer info
        strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

template <typename T>
struct track_functor_t : apply_functor_t
{
    T& data;

    track_functor_t(T& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { tracking::track(archive, data); }
};

template <typename T>
struct raw_functor_t : apply_functor_t
{
    T& data;

    raw_functor_t(T& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { tracking::raw(archive, data); }
};

} // namespace apply

namespace tracking
{

template <typename T> apply::track_functor_t<T> track(T& data) noexcept { return { data }; }
template <typename T> apply::raw_functor_t<T> raw(T& data) noexcept { return { data }; }

} // namespace tracking

} // namespace sf

#include <valarray> // valarray

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

} // namespace utility

} // namespace sf

namespace sf
{

namespace compress
{

// always require compressible type for fast compression
template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_compressible<T>>::value)>
void fast(ArchiveType& archive, T& object)
{
    using item_type = typename meta::value<T>::type;

    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::is_ioarchive<ArchiveType>::value)>
void slow(ArchiveType& archive, T& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_compressible<T>>::value)>
void zip(ArchiveType& archive, T& object)
{
    fast(archive, object);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::negation<meta::is_compressible<T>>>::value)>
void zip(ArchiveType& archive, T& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, number, std::is_arithmetic<T>::value)
{
    ::sf::binary(archive, number);
}

CONDITIONAL_SERIALIZATION(save, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    archive & value;
}

CONDITIONAL_SERIALIZATION(load, enumerator, std::is_enum<T>::value)
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff{};
    archive & buff;

    enumerator = static_cast<T>(buff);
}

CONDITIONAL_SERIALIZATION(saveload, array, std::is_array<T>::value)
{
    ::sf::compress::zip(archive, array);
}

CONDITIONAL_SERIALIZATION(saveload, pointer, ::sf::meta::is_serializable_raw_pointer<T>::value)
{
#ifdef SF_PTRTRACK_DISABLE
    ::sf::tracking::raw(archive, pointer);
#else
    ::sf::tracking::track(archive, pointer);
#endif // SF_PTRTRACK_DISABLE
}

#if __cplusplus >= 201703L

#define SF_AGGREGATE_IMPLEMENTATION_GENERIC(count)                                                      \
    template <class ArchiveType, typename T>                                                            \
    void aggregate_impl(ArchiveType& archive, T& object, std::integral_constant<std::size_t, count>) {  \
        auto& [SF_PLACEHOLDERS(count)] = object;                                                        \
        archive(SF_PLACEHOLDERS(count));                                                                \
    }

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_aggregate
    : all<is_aggregate<T>, negation<std::is_union<T>>> {};

} // namespace meta

namespace detail
{

template <class ArchiveType, typename T>
void aggregate_impl(ArchiveType& archive, T& object, std::integral_constant<std::size_t, 0>) noexcept { /*pass*/ }

SF_REPEAT(SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_aggregate<T>>::value)>
void aggregate(ArchiveType& archive, T& object)
{
    constexpr auto size = meta::aggregate_size<T>::value;
    detail::aggregate_impl(archive, object, std::integral_constant<std::size_t, size>{});
}

namespace apply
{

template <typename T>
struct aggregate_functor_t : apply_functor_t
{
    T& object;

    aggregate_functor_t(T& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { aggregate(archive, object); }
};

} // namespace apply

template <typename T> apply::aggregate_functor_t<T> aggregate(T& object) noexcept { return { object }; }

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, object, ::sf::meta::is_serializable_aggregate<T>::value)
{
    ::sf::aggregate(archive, object);
}

// clean up
#undef SF_AGGREGATE_IMPLEMENTATION_GENERIC

#endif // if

CONDITIONAL_SERIALIZATION(saveload, data, std::is_union<T>::value)
{
    ::sf::binary(archive, data);
}

namespace sf
{

template <class Base, class ArchiveType, class Derived,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               std::is_base_of<Base, Derived>>::value)>
void base(ArchiveType& archive, Derived& object)
{
    ::xxsf::serialize_base<Base>(archive, object);
}

template <class Base, class ArchiveType, class Derived,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               std::is_base_of<Base, Derived>>::value)>
void virtual_base(ArchiveType& archive, Derived& object)
{
#ifdef SF_PTRTRACK_DISABLE
    if (::xxsf::traits(object) == ::xxsf::template traits<Derived>())
        base<Base>(archive, object);
#else
    using key_type = typename ArchiveType::TrackingKeyType;

    auto address = memory::pure(std::addressof(object));

    const auto key = reinterpret_cast<key_type>(address);
    const auto traits = ::xxsf::traits<Base>();

    auto& hierarchy_tracking = archive.template tracking<tracking::hierarchy_t>();

    auto& is_tracking = hierarchy_tracking[{key, traits}];
    if (not is_tracking)
    {
        is_tracking = true;
        base<Base>(archive, object);
    }
#endif // SF_PTRTRACK_DISABLE
}

namespace detail
{

template <class Base, class ArchiveType, class Derived,
          SF_REQUIRE(not meta::is_virtual_base_of<Base, Derived>::value)>
void native_base(ArchiveType& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class ArchiveType, class Derived,
          SF_REQUIRE(meta::is_virtual_base_of<Base, Derived>::value)>
void native_base(ArchiveType& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <class Derived, class Base>
struct base_functor_t : apply_functor_t
{
    Derived& object;

    base_functor_t(Derived& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { base<Base>(archive, object); }
};

template <class Derived, class Base>
struct virtual_base_functor_t : apply_functor_t
{
    Derived& object;

    virtual_base_functor_t(Derived& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { virtual_base<Base>(archive, object); }
};

} // namespace apply

template <class Base, class Derived,
          SF_REQUIRE(std::is_base_of<Base, Derived>::value)>
apply::base_functor_t<Derived, Base> base(Derived& object) noexcept { return { object }; }

template <class Base, class Derived,
          SF_REQUIRE(std::is_base_of<Base, Derived>::value)>
apply::virtual_base_functor_t<Derived, Base> virtual_base(Derived& object) noexcept { return { object }; }

// default empty impl
template <class ArchiveType, class Derived>
void hierarchy(ArchiveType& archive, Derived& object) noexcept { /*pass*/ }

// Variadic native_base function
template <class Base, class... Base_n, class ArchiveType, class Derived,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_derived_of<Derived, Base, Base_n...>>::value)>
void hierarchy(ArchiveType& archive, Derived& object)
{
    detail::native_base<Base>(archive, object);
    hierarchy<Base_n...>(archive, object);
}

namespace apply
{

template <class Derived, class Base, class... Base_n>
struct hierarchy_functor_t : apply_functor_t
{
    Derived& object;

    hierarchy_functor_t(Derived& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { hierarchy<Base, Base_n...>(archive, object); }
};

} // namespace apply

template <class Base, class... Base_n, class Derived,
          SF_REQUIRE(meta::is_derived_of<Derived, Base, Base_n...>::value)>
apply::hierarchy_functor_t<Derived, Base, Base_n...> hierarchy(Derived& object) noexcept
{
    return { object };
}

} // namespace sf

// TODO: remove xxsf
#define SERIALIZATION_ACCESS(...)                                                                       \
    friend struct ::xxsf;                                                                               \
    template <typename, typename> friend struct ::xxsf_save;                                            \
    template <typename, typename> friend struct ::xxsf_load;                                            \
    template <typename, typename> friend struct ::xxsf_saveload;

// Alternative instantiable registration with library traits no-rtti
#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        SERIALIZATION_FIXTURE(__VA_ARGS__)                                                              \
        SERIALIZATION_TRAITS(__VA_ARGS__)
#endif // SERIALIZABLE

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename T> void serializable()
{
    static_assert(not meta::is_unsupported<T>::value, "The 'T' is an unsupported type for serialization.");

    dynamic::instantiable_fixture_t<T>::call();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::any_fixture_t<T>::call();
#endif // if
}

template <typename T> T&& serializable(T&& object)
{
    serializable<typename std::decay<T>::type>();
    return std::forward<T>(object);
}

} // namepsace sf

EXPORT_SERIALIZATION_ARCHIVE(0, i, ::sf::iarchive_t<::sf::wrapper::ibyte_stream_t<>>)
EXPORT_SERIALIZATION_ARCHIVE(0, o, ::sf::oarchive_t<::sf::wrapper::obyte_stream_t<>>)

#ifndef SF_DEFAULT_DISABLE

EXPORT_SERIALIZATION_ARCHIVE(1, i, ::sf::iarchive_t<::sf::wrapper::ifile_stream_t<>>)
EXPORT_SERIALIZATION_ARCHIVE(1, o, ::sf::oarchive_t<::sf::wrapper::ofile_stream_t<>>)

#endif // SF_DEFAULT_DISABLE

#include <tuple> // tuple

namespace sf
{

template <typename ElementType>
class alias_t
{
private:
    ElementType* data_;

public:
    using element_type = ElementType;

    // DONT use dereferencing of null data before rebinding
    alias_t() noexcept : data_(nullptr) {}

    template <typename dT,
              SF_REQUIRE(meta::is_static_castable<dT*, element_type*>::value)>
    alias_t(dT& data) noexcept
        : data_(std::addressof(data)) {}

    template <typename dT>
    alias_t(const alias_t<dT>& data) noexcept : alias_t(data.get()) {}

public:
    // rebinding data
    alias_t(const alias_t&) = default;
    alias_t& operator=(const alias_t&) = default;

    bool is_refer() const noexcept { return data_ != nullptr; }

    template <typename dT>
    bool is_refer(dT& data)  const noexcept { return data_ == std::addressof(data); }

    operator element_type&() const noexcept { return get(); }

    element_type& get() const noexcept { return *data_; }
    void set(element_type& data) noexcept { data_ = std::addressof(data); }
};

} // namespace sf

TEMPLATE_SERIALIZATION(save, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    if (not alias.is_refer())
        throw "The write alias_t must be initialized.";

    auto pointer = std::addressof(alias.get());
    const auto key = ::sf::detail::refer_key(archive, pointer);

    auto& is_tracking = archive.template tracking<::sf::tracking::raw_t>()[key];

    if (not is_tracking)
        throw "The write alias_t must be tracked before.";

    ::sf::detail::native_save(archive, pointer, key);
}

TEMPLATE_SERIALIZATION(load, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
    using key_type = typename ArchiveType::TrackingKeyType;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (alias.is_refer())
        throw "The read alias_t must be null.";
#endif // SF_GARBAGE_CHECK_DISABLE

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<::sf::tracking::raw_t>()[key];

    if (item.address == nullptr)
        throw "The read alias_t must be tracked before.";

    ElementType* pointer = nullptr;

    ::sf::detail::native_load(archive, pointer, item.address);

    alias.set(*pointer); // pointer will never nullptr
}

namespace sf
{

namespace utility
{

template <typename T, std::size_t N>
class span_t;

template <typename T, std::size_t N>
class span_base_t
{
public:
    using size_type         = std::size_t;
    using value_type        = span_t<T, N - 1>;

    using pointer           = typename meta::pointer<T, N>::type;
    using const_pointer     = const pointer;

protected:
    using Dimension         = size_type[N];

protected:
    alias_t<pointer> data_;
    Dimension dim_;

protected:
    span_base_t(pointer& data) noexcept
        : data_(data), dim_() {}

    template <typename D, typename... Dn>
    span_base_t(pointer& data, D d, Dn... dn) noexcept
        : data_(data), dim_{d, dn...} {}

public:
    void init(pointer data) noexcept { data_.get() = data; }
    void data(alias_t<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    size_type size() const noexcept { return dim_[0]; }
    Dimension& dim() noexcept { return dim_; }
};

template <typename T, std::size_t N>
class span_t : public span_base_t<T, N>
{
protected:
    using Core = span_base_t<T, N>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = span_t<T, N - 1>;
    using dereference_type  = typename meta::pointer<T, N - 1>::type;

    using reference         = value_type&;
    using const_reference   = const value_type&;

protected:
    using typename Core::Dimension;

private:
    mutable value_type child_scope_;

public:
    span_t(pointer& data, Dimension dim) noexcept
        : Core(data), child_scope_(data[0], dim + 1)
    {
        for (size_type i = 0; i < N; ++i)
            this->dim_[i] = dim;
    }

    template <typename D, typename... Dn,
              SF_REQUIRE(not std::is_array<D>::value)>
    span_t(pointer& data, D d, Dn... dn) noexcept
        : Core(data, d, dn...), child_scope_(data[0], dn...) {}

    void size(size_type value) noexcept { this->dim_[0] = value; }

    reference operator[] (size_type i) noexcept
    {
        child_scope_.data(this->data_[i]);
        return child_scope_;
    }

public:
    using Core::size; // prevent Core function hiding
};

template <typename T>
class span_t<T, 1> : public span_base_t<T, 1>
{
protected:
    using Core = span_base_t<T, 1>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = T;
    using dereference_type  = T;

    using reference         = T&;
    using const_reference   = const T&;

protected:
    using typename Core::Dimension;

public:
    span_t(pointer& data, Dimension size) noexcept
        : Core(data)
    {
        this->dim_[0] = size[0];
    }

    span_t(pointer& data, size_type size) noexcept
        : Core(data, size) {}

    reference operator[] (size_type i) noexcept { return this->data_[i]; }
};

} // namespace utility

namespace meta
{

template <typename> struct is_span : std::false_type {};
template <typename T, std::size_t N>
struct is_span<utility::span_t<T, N>> : std::true_type {};

template <typename Pointer, typename D, typename... Dn>
struct is_span_set
    : meta::all<std::integral_constant<bool, meta::pointer_count<Pointer>::value >= sizeof...(Dn) + 1>,
                meta::all<std::is_arithmetic<D>,
                          std::is_arithmetic<Dn>...>> {};

} // namespace meta

namespace utility
{

template <typename Pointer, typename D, typename... Dn,
          std::size_t N = sizeof...(Dn) + 1,
          typename Type = typename meta::remove_pointer<Pointer, N>::type,
          SF_REQUIRE(meta::is_span_set<Pointer, D, Dn...>::value)>
span_t<Type, N> make_span(Pointer& data, D d, Dn... dn)
{
    using size_type = typename span_t<Type, N>::size_type;
    return { data, static_cast<size_type>(d), static_cast<size_type>(dn)... };
}

} // namespace utility

namespace detail
{

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::negation<meta::is_span<T>>>::value)>
void span_impl(ArchiveType& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_span<T>>::value)>
void span_impl(ArchiveType& archive, T& array)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_span<T>>::value)>
void span_impl(ArchiveType& archive, T& array)
{
    using size_type = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    auto ptr = new dereference_type [array.size()] {};
    array.init(ptr);

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class ArchiveType, typename T,
          typename D, typename... Dn,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_span_set<T, D, Dn...>>::value)>
void span(ArchiveType& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (not detail::refer_key(archive, pointer)) return; // serialize refer info
    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::span_impl(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename T, typename D, typename... Dn>
struct span_functor_t : apply_functor_t
{
    using PackType = std::tuple<T&, D&, Dn&...>;

    PackType pack;

    span_functor_t(T& pointer, D& d, Dn&... dn) noexcept : pack(pointer, d, dn...) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const
    {
        invoke(archive, meta::make_index_sequence<std::tuple_size<PackType>::value>{});
    }

private:
    template <class ArchiveType, std::size_t... I>
    void invoke(ArchiveType& archive, meta::index_sequence<I...>) const
    {
        span(archive, std::get<I>(pack)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename T, typename D, typename... Dn,
          SF_REQUIRE(meta::is_span_set<T, D, Dn...>::value)>
apply::span_functor_t<T, D, Dn...> span(T& pointer, D& dimension, Dn&... dimension_n) noexcept
{
    return { pointer, dimension, dimension_n... };
}

} // inline namespace common

} // namespace sf

#define SF_BITPACK(common_fields_type, archive, ...)                                                    \
    {                                                                                                   \
        auto xxbitpack = ::sf::bitpack<common_fields_type>(archive);                                    \
        SF_CONCAT(SF_BITPACK_, SF_VA_ARGS_SIZE(__VA_ARGS__))(__VA_ARGS__)                               \
    }

#define SF_BITFIELD(field_and_bits) SF_FIRST_ARGUMENT field_and_bits = xxbitpack field_and_bits;
#define SF_BITPACK_1(field_and_bits) SF_BITFIELD(field_and_bits)
#define SF_BITPACK_2(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_1(__VA_ARGS__)
#define SF_BITPACK_3(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_2(__VA_ARGS__)
#define SF_BITPACK_4(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_3(__VA_ARGS__)
#define SF_BITPACK_5(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_4(__VA_ARGS__)
#define SF_BITPACK_6(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_5(__VA_ARGS__)
#define SF_BITPACK_7(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_6(__VA_ARGS__)
#define SF_BITPACK_8(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_7(__VA_ARGS__)

#define SF_BITPACK_9(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_8(__VA_ARGS__)
#define SF_BITPACK_10(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_9(__VA_ARGS__)
#define SF_BITPACK_11(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_10(__VA_ARGS__)
#define SF_BITPACK_12(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_11(__VA_ARGS__)
#define SF_BITPACK_13(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_12(__VA_ARGS__)
#define SF_BITPACK_14(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_13(__VA_ARGS__)
#define SF_BITPACK_15(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_14(__VA_ARGS__)
#define SF_BITPACK_16(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_15(__VA_ARGS__)

#define SF_BITPACK_17(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_16(__VA_ARGS__)
#define SF_BITPACK_18(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_17(__VA_ARGS__)
#define SF_BITPACK_19(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_18(__VA_ARGS__)
#define SF_BITPACK_20(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_19(__VA_ARGS__)
#define SF_BITPACK_21(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_20(__VA_ARGS__)
#define SF_BITPACK_22(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_21(__VA_ARGS__)
#define SF_BITPACK_23(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_22(__VA_ARGS__)
#define SF_BITPACK_24(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_23(__VA_ARGS__)

#define SF_BITPACK_25(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_24(__VA_ARGS__)
#define SF_BITPACK_26(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_25(__VA_ARGS__)
#define SF_BITPACK_27(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_26(__VA_ARGS__)
#define SF_BITPACK_28(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_27(__VA_ARGS__)
#define SF_BITPACK_29(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_28(__VA_ARGS__)
#define SF_BITPACK_30(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_29(__VA_ARGS__)
#define SF_BITPACK_31(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_30(__VA_ARGS__)
#define SF_BITPACK_32(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_31(__VA_ARGS__)

#define SF_BITPACK_33(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_32(__VA_ARGS__)
#define SF_BITPACK_34(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_33(__VA_ARGS__)
#define SF_BITPACK_35(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_34(__VA_ARGS__)
#define SF_BITPACK_36(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_35(__VA_ARGS__)
#define SF_BITPACK_37(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_36(__VA_ARGS__)
#define SF_BITPACK_38(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_37(__VA_ARGS__)
#define SF_BITPACK_39(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_38(__VA_ARGS__)
#define SF_BITPACK_40(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_39(__VA_ARGS__)

#define SF_BITPACK_41(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_40(__VA_ARGS__)
#define SF_BITPACK_42(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_41(__VA_ARGS__)
#define SF_BITPACK_43(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_42(__VA_ARGS__)
#define SF_BITPACK_44(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_43(__VA_ARGS__)
#define SF_BITPACK_45(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_44(__VA_ARGS__)
#define SF_BITPACK_46(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_45(__VA_ARGS__)
#define SF_BITPACK_47(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_46(__VA_ARGS__)
#define SF_BITPACK_48(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_47(__VA_ARGS__)

#define SF_BITPACK_49(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_48(__VA_ARGS__)
#define SF_BITPACK_50(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_49(__VA_ARGS__)
#define SF_BITPACK_51(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_50(__VA_ARGS__)
#define SF_BITPACK_52(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_51(__VA_ARGS__)
#define SF_BITPACK_53(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_52(__VA_ARGS__)
#define SF_BITPACK_54(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_53(__VA_ARGS__)
#define SF_BITPACK_55(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_54(__VA_ARGS__)
#define SF_BITPACK_56(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_55(__VA_ARGS__)

#define SF_BITPACK_57(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_56(__VA_ARGS__)
#define SF_BITPACK_58(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_57(__VA_ARGS__)
#define SF_BITPACK_59(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_58(__VA_ARGS__)
#define SF_BITPACK_60(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_59(__VA_ARGS__)
#define SF_BITPACK_61(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_60(__VA_ARGS__)
#define SF_BITPACK_62(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_61(__VA_ARGS__)
#define SF_BITPACK_63(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_62(__VA_ARGS__)
#define SF_BITPACK_64(field_and_bits, ...) SF_BITFIELD(field_and_bits) SF_BITPACK_63(__VA_ARGS__)
// and etc.

// Signature:
// BITPACK(common_fields_type, archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
// BITPACK macro will generate code:
// {
//     auto xxbitpack = ::sf::bitpack<common_fields_type>(archive);
//     object.field0 = xxbitpack(object.field0, field0_bits);
//     object.field1 = xxbitpack(object.field1, field1_bits);
//     and etc.
// }
#define BITPACK(common_fields_type, archive, ...) SF_BITPACK(common_fields_type, archive, __VA_ARGS__)

namespace sf
{

namespace detail
{

template <class ArchiveType, typename T, typename enable = void>
struct bitpack_t;

template <class ArchiveType, typename T>
struct bitpack_t<ArchiveType, T, typename std::enable_if<sf::meta::is_oarchive<ArchiveType>::value>::type>
{
    ArchiveType& archive;
    T data{};
    std::size_t offset{};

    bitpack_t(ArchiveType& archive) : archive(archive) {}
    ~bitpack_t() { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        // same as data = data | (field << offset);
        data |= field << offset;
        offset += bits;

        return field;
    }
};

template <class ArchiveType, typename T>
struct bitpack_t<ArchiveType, T, typename std::enable_if<sf::meta::is_iarchive<ArchiveType>::value>::type>
{
    ArchiveType& archive;
    T data{};

    bitpack_t(ArchiveType& archive) : archive(archive) { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        //same as field = data & ~(0xf...f << bits)
        field = data & ~(~T{} << bits);
        data >>= bits;

        return field;
    }
};

} // namespace detail

template <typename PackType = let::u32, class ArchiveType>
detail::bitpack_t<ArchiveType, PackType> bitpack(ArchiveType& archive) noexcept { return { archive }; }

} // namespace sf

TEMPLATE_SERIALIZATION(save, vector,
    (template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = vector.size();
    archive & size;

    ::sf::compress::zip(archive, vector);
}

TEMPLATE_SERIALIZATION(load, vector,
    (template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    vector.resize(size);
    ::sf::compress::zip(archive, vector);
}

// slow impl
SERIALIZATION(save, vector, std::vector<bool>)
{
    ::sf::let::u64 size = vector.size();
    archive & size;

    for(auto item:vector)
    {
        bool boolean = item;
        archive & boolean;
    }
}

SERIALIZATION(load, vector, std::vector<bool>)
{
    ::sf::let::u64 size{};
    archive & size;

    vector.resize(size);

    for(auto item/*bit_reference*/:vector)
    {
        bool boolean{};
        archive & boolean;
        item = boolean;
    }
}

TEMPLATE_SERIALIZATION(saveload, array, (template <typename ValueType, std::size_t BitsetSize>), std::array<ValueType, BitsetSize>)
{
    ::sf::compress::zip(archive, array);
}

#include <string> // basic_string

TEMPLATE_SERIALIZATION(save, string,
    (template <typename CharType, typename Traits, typename AllocatorType>),
    std::basic_string<CharType, Traits, AllocatorType>)
{
    ::sf::let::u64 size = string.size();
    archive & size;

    ::sf::compress::zip(archive, string);
}

TEMPLATE_SERIALIZATION(load, string,
    (template <typename CharType, typename Traits, typename AllocatorType>),
    std::basic_string<CharType, Traits, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    string.resize(size);
    ::sf::compress::zip(archive, string);
}

TEMPLATE_SERIALIZATION(saveload, pair, (template <typename FirstType, typename SecondType>), std::pair<FirstType, SecondType>)
{
    archive & pair.first & pair.second;
}

namespace sf
{

namespace detail
{

template <class ArchiveType, typename... ArgumentTypes, std::size_t... I>
void expand(ArchiveType& archive, std::tuple<ArgumentTypes...>& tuple, meta::index_sequence<I...>)
{
    archive(std::get<I>(tuple)...);
}

} // namespace detail

} // namespace sf

TEMPLATE_SERIALIZATION(saveload, tuple, (template <typename... ArgumentTypes>), std::tuple<ArgumentTypes...>)
{
    ::sf::detail::expand(archive, tuple, ::sf::meta::make_index_sequence<sizeof...(ArgumentTypes)>{});
}

#include <list> // list

TEMPLATE_SERIALIZATION(save, list, (template <typename ValueType, typename AllocatorType>), std::list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = list.size();
    archive & size;

    ::sf::compress::slow(archive, list);
}

TEMPLATE_SERIALIZATION(load, list, (template <typename ValueType, typename AllocatorType>), std::list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    list.resize(size);
    ::sf::compress::slow(archive, list);
}

#include <forward_list> // forward_list

TEMPLATE_SERIALIZATION(save, forward_list,
    (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = std::distance(forward_list.begin(), forward_list.end());
    archive & size;

    ::sf::compress::slow(archive, forward_list);
}

TEMPLATE_SERIALIZATION(load, forward_list,
    (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    forward_list.resize(size);
    ::sf::compress::slow(archive, forward_list);
}

#include <set> // set, multiset
#include <unordered_set> // unordered_set, unordered_multiset

#define SF_IS_STD_SET_TYPE_META_GENERIC(set_type)                                                       \
    template <typename> struct is_std_##set_type : std::false_type {};                                  \
    template <typename KeyType, typename Comparator, typename AllocatorType>                            \
    struct is_std_##set_type<std::set_type<KeyType, Comparator, AllocatorType>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_SET_TYPE_META_GENERIC(set)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_set)
SF_IS_STD_SET_TYPE_META_GENERIC(multiset)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_multiset)

template <class T> struct is_std_any_unordered_set
    : one<is_std_unordered_set<T>,
          is_std_unordered_multiset<T>> {};

template <class T> struct is_std_any_set
    : one<is_std_set<T>,
          is_std_multiset<T>,
          is_std_any_unordered_set<T>> {};

} // namespace meta

namespace detail
{

template <class T,
          SF_REQUIRE(not meta::is_std_any_unordered_set<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SF_REQUIRE(meta::is_std_any_unordered_set<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, set, ::sf::meta::is_std_any_set<T>::value)
{
    ::sf::let::u64 size = set.size();
    archive & size;

    ::sf::compress::slow(archive, set);
}

CONDITIONAL_SERIALIZATION(load, set, ::sf::meta::is_std_any_set<T>::value)
{
    using value_type = typename T::value_type;

    ::sf::let::u64 size{};
    archive & size;

    set.clear();
    ::sf::detail::reserve_unordered(set, size);

    auto hint = set.begin();
    for (::sf::let::u64 i = 0; i < size; ++i)
    {
        value_type item{}; // temp
        archive & item;

        hint = set.emplace_hint(hint, std::move(item));
    }
}

// clean up
#undef SF_IS_STD_SET_TYPE_META_GENERIC

TEMPLATE_SERIALIZATION(save, unique_ptr,
    (template <typename ElementType, typename Deleter>), std::unique_ptr<ElementType, Deleter>)
{
    auto data = unique_ptr.get();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, unique_ptr,
    (template <typename ElementType, typename Deleter>), std::unique_ptr<ElementType, Deleter>)
{
    ElementType* data = nullptr;
    archive & data;

    unique_ptr.reset(data);
}

TEMPLATE_SERIALIZATION(saveload, shared_ptr, template <typename ElementType>, std::shared_ptr<ElementType>)
{
    ::sf::tracking::track(archive, shared_ptr);
}

// serialization of shared_ptr

TEMPLATE_SERIALIZATION(save, weak_ptr, template <typename ElementType>, std::weak_ptr<ElementType>)
{
    auto sptr = weak_ptr.lock();
    archive & sptr;
}

TEMPLATE_SERIALIZATION(load, weak_ptr, template <typename ElementType>, std::weak_ptr<ElementType>)
{
    std::shared_ptr<ElementType> sptr;
    archive & sptr;

    weak_ptr = sptr;
}

#include <map> // map, multimap

// serialization of core map value_type

#define SF_IS_STD_MAP_TYPE_META_GENERIC(map_type)                                                       \
    template <typename> struct is_std_##map_type : std::false_type {};                                  \
    template <typename KeyType, typename ValueType, typename Comparator, typename AllocatorType>        \
    struct is_std_##map_type<std::map_type<KeyType, ValueType, Comparator, AllocatorType>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_MAP_TYPE_META_GENERIC(map)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
SF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class T> struct is_std_any_unordered_map
    : one<is_std_unordered_map<T>,
          is_std_unordered_multimap<T>> {};

template <class T> struct is_std_any_map
    : one<is_std_map<T>,
          is_std_multimap<T>,
          is_std_any_unordered_map<T>> {};

} // namespace meta

namespace detail
{

template <class T,
          SF_REQUIRE(not meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& ordered, std::size_t size) noexcept { /*pass*/ }

template <class T,
          SF_REQUIRE(meta::is_std_any_unordered_map<T>::value)>
void reserve_unordered(T& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, map, ::sf::meta::is_std_any_map<T>::value)
{
    ::sf::let::u64 size = map.size();
    archive & size;

    ::sf::compress::slow(archive, map);
}

CONDITIONAL_SERIALIZATION(load, map, ::sf::meta::is_std_any_map<T>::value)
{
    using key_type   = typename T::key_type;
    using value_type = typename T::mapped_type;

    ::sf::let::u64 size{};
    archive & size;

    map.clear();
    ::sf::detail::reserve_unordered(map, size);

    auto hint = map.begin();
    for (::sf::let::u64 i = 0; i < size; ++i)
    {
        key_type key{};
        value_type value{};

        archive & key & value;

        hint = map.emplace_hint(hint, std::move(key), std::move(value));
    }
}

//clear
#undef SF_IS_STD_MAP_TYPE_META_GENERIC

#include <deque> // deque

TEMPLATE_SERIALIZATION(save, deque, (template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = deque.size();
    archive & size;

    ::sf::compress::slow(archive, deque);
}

TEMPLATE_SERIALIZATION(load, deque, (template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    deque.resize(size);
    ::sf::compress::slow(archive, deque);
}

#include <stack> // stack

namespace sf
{

namespace meta
{

template <template <typename...> class Adapter,
          typename Type, class Container, typename... Args>
Container& underlying(Adapter<Type, Container, Args...>& adapter) noexcept
{
    using Core = Adapter<Type, Container, Args...>;

    struct base_inner : public Core
    {
        static Container& underlying(Core& core)
        {
            auto underlying_container = &base_inner::c;
            return core.*underlying_container;
        }
    };

    return base_inner::underlying(adapter);
}

} // namespace meta

} // namespace sf

// default container for stack

TEMPLATE_SERIALIZATION(saveload, stack,
    (template <typename ValueType, class ContainerType>), std::stack<ValueType, ContainerType>)
{
    archive & ::sf::meta::underlying(stack);
}

#include <queue> // queue

// default container for queue

TEMPLATE_SERIALIZATION(saveload, queue,
    (template <typename ValueType, class ContainerType>), std::queue<ValueType, ContainerType>)
{
    archive & ::sf::meta::underlying(queue);
}

TEMPLATE_SERIALIZATION(save, valarray, template <typename ValueType>, std::valarray<ValueType>)
{
    ::sf::let::u64 size = valarray.size();
    archive & size;

    ::sf::compress::zip(archive, valarray);
}

TEMPLATE_SERIALIZATION(load, valarray, template <typename ValueType>, std::valarray<ValueType>)
{
    ::sf::let::u64 size{};
    archive & size;

    valarray.resize(size);
    ::sf::compress::zip(archive, valarray);
}

#include <bitset> // bitset

// default array for bitset convertion

// slow impl
TEMPLATE_SERIALIZATION(save, bitset, template <std::size_t BitsetSize>, std::bitset<BitsetSize>)
{
    auto data = bitset.to_string();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, bitset, template <std::size_t BitsetSize>, std::bitset<BitsetSize>)
{
    std::string data;
    archive & data;

    bitset = std::bitset<BitsetSize>(data);
}

#include <atomic> // atomic

TEMPLATE_SERIALIZATION(save, atomic, template <typename ValueType>, std::atomic<ValueType>)
{
    auto object = atomic.load();
    archive & object;
}

TEMPLATE_SERIALIZATION(load, atomic, template <typename ValueType>, std::atomic<ValueType>)
{
    ValueType value{};
    archive & value;

    atomic.store(value);
}

#include <complex> // complex

TEMPLATE_SERIALIZATION(save, complex, template <typename ValueType>, std::complex<ValueType>)
{
    auto re = complex.real();
    auto im = complex.imag();

    archive & re & im;
}

TEMPLATE_SERIALIZATION(load, complex, template <typename ValueType>, std::complex<ValueType>)
{
    ValueType re{};
    ValueType im{};

    archive & re & im;

    complex.real(re);
    complex.imag(im);
}

#if __cplusplus >= 201703L

#include <optional> // optional

TEMPLATE_SERIALIZATION(save, optional, (template <typename ValueType>), std::optional<ValueType>)
{
    auto is_init = optional.has_value();
    archive & is_init;

    if (is_init) archive & optional.value();
}

TEMPLATE_SERIALIZATION(load, optional, (template <typename ValueType>), std::optional<ValueType>)
{
    auto is_init = false;
    archive & is_init;

    if (is_init)
    {
        optional.emplace();
        archive & optional.value();
    }
}

#endif // if

#if __cplusplus >= 201703L

#include <variant> // variant

// serialization of std::monostate

namespace sf
{

namespace detail
{

template <let::u64 I, class ArchiveType, class Variant,
          SF_REQUIRE(I == std::variant_size<Variant>::value)>
void variant_save(ArchiveType& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class ArchiveType, class Variant,
          SF_REQUIRE(I < std::variant_size<Variant>::value)>
void variant_save(ArchiveType& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_save<I + 1>(archive, variant, index);
    archive & std::get<I>(variant);
}

template <typename Type, class ArchiveType, class Variant,
          SF_REQUIRE(not std::is_constructible<Type>::value)>
void variant_load_impl(ArchiveType& archive, Variant& variant)
{
    throw "Require default constructor for specify type.";
}

template <typename Type, class ArchiveType, class Variant,
          SF_REQUIRE(std::is_constructible<Type>::value)>
void variant_load_impl(ArchiveType& archive, Variant& variant)
{
    archive & variant.template emplace<Type>();
}

template <let::u64 I, class ArchiveType, class Variant,
          SF_REQUIRE(I == std::variant_size<Variant>::value)>
void variant_load(ArchiveType& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class ArchiveType, class Variant,
          SF_REQUIRE(I < std::variant_size<Variant>::value)>
void variant_load(ArchiveType& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_load<I + 1>(archive, variant, index);

    using type = typename std::variant_alternative<I, Variant>::type;
    variant_load_impl<type>(archive, variant);
}

} // namespace detail

} // namespace sf

TEMPLATE_SERIALIZATION(save, variant, (template <typename... ArgumentTypes>), std::variant<ArgumentTypes...>)
{
    ::sf::let::u64 index = variant.index();
    archive & index;

    if (index != std::variant_npos)
        ::sf::detail::variant_save(archive, variant, index);
}

TEMPLATE_SERIALIZATION(load, variant, (template <typename... ArgumentTypes>), std::variant<ArgumentTypes...>)
{
    ::sf::let::u64 index{};
    archive & index;

    if (index != std::variant_npos)
        ::sf::detail::variant_load(archive, variant, index);
}

#endif // if

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

// please, use 'sf::serializable' for type any registry before std::any serialization
SERIALIZATION(save, any, std::any)
{
    ::sf::let::u64 hash = SF_TYPE_HASH(any.type());
    archive & hash;

    ::sf::dynamic::any_registry_t::instance().save(archive, any, hash);
}

SERIALIZATION(load, any, std::any)
{
    ::sf::let::u64 hash{};
    archive & hash;

    ::sf::dynamic::any_registry_t::instance().load(archive, any, hash);
}

#endif // if

#endif // SERIALIZATION_FIXTURE_HPP
