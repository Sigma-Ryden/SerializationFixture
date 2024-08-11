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
static constexpr auto xxsf_archive_traits_base_key = ::xxsf_archive_traits_key_type(-1);

template <class ArchiveType>
struct xxsf_archive_traits
{
    static constexpr auto key = xxsf_archive_traits_base_key;
};

template <::xxsf_archive_traits_key_type ArchiveKey>
struct xxsf_iarchive_registry { using type = ::sf::core::ioarchive_t; };

template <::xxsf_archive_traits_key_type ArchiveKey>
struct xxsf_oarchive_registry { using type = ::sf::core::ioarchive_t; };

#include <cstddef> // size_t

#include <type_traits>
// is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#if __cplusplus >= 201703L
#include <any> // any
#endif // if

#include <memory> // shared_ptr
#include <array> // array

#define SF_REQUIRES(...) typename std::enable_if<(bool)(__VA_ARGS__), int>::type = 0

namespace sf
{

namespace meta
{

template <typename...> constexpr bool to_false() noexcept { return false; }

template <typename Type, std::size_t IndexableValue = 0>
auto declval() noexcept -> decltype(std::declval<Type>()) { return std::declval<Type>(); }

template <class...> struct all : std::true_type {};
template <class ConditionType> struct all<ConditionType> : ConditionType {};
template <class ConditionType, class... OtherConditionTypes>
struct all<ConditionType, OtherConditionTypes...>
    : std::conditional<bool(ConditionType::value), all<OtherConditionTypes...>, ConditionType>::type {};

template <class...> struct one : std::false_type {};
template <class ConditionType> struct one<ConditionType> : ConditionType {};
template <class ConditionType, class... OtherConditionTypes>
struct one<ConditionType, OtherConditionTypes...>
    : std::conditional<bool(ConditionType::value), ConditionType, one<OtherConditionTypes...>>::type {};

template <class ConditionType> struct negation : std::integral_constant<bool, not bool(ConditionType::value)> {};

template <typename Type, typename... OtherTypes> struct is_same : all<std::is_same<Type, OtherTypes>...> {};

template <typename...> using void_t = void;

template <typename Type, std::size_t PointeringNumberValue = 1> struct remove_pointer { using type = Type; };
template <typename Type> struct remove_pointer<Type*, 1> { using type = Type; };

template <typename Type, std::size_t PointeringNumberValue>
struct remove_pointer<Type*, PointeringNumberValue>
{
    using type = typename remove_pointer<Type, PointeringNumberValue - 1>::type;
};

template <typename Type, typename = void_t<>> struct dereference { using type = Type; };
template <typename Type> struct dereference<Type*> { using type = Type; };
template <typename Type> struct dereference<std::weak_ptr<Type>> { using type = Type; };
template <typename Type> struct dereference<std::shared_ptr<Type>> { using type = Type; };
template <typename Type> struct dereference<std::unique_ptr<Type>> { using type = Type; };
template <typename Type> struct dereference<Type, void_t<decltype(*std::declval<Type>())>>
    : std::remove_reference<decltype(*std::declval<Type>())> {};

template <typename Type, std::size_t PointeringNumberValue = 1>
struct pointer { using type = typename pointer<Type, PointeringNumberValue - 1>::type*; };
template <typename Type> struct pointer<Type, 0> { using type = Type; };

 // limited by template depth
template <std::size_t... SequenceValues> struct index_sequence
{
    static constexpr auto value = sizeof...(SequenceValues);
};

namespace detail
{

template <std::size_t SequenceValue, std::size_t... SequenceValues>
struct index_sequence_helper : index_sequence_helper<SequenceValue - 1, SequenceValue - 1, SequenceValues...> {};
template <std::size_t... SequenceValues>
struct index_sequence_helper<0, SequenceValues...> { using type = index_sequence<SequenceValues...>; };

} // namespace detail

template <std::size_t SequenceSizeValue>
using make_index_sequence = typename detail::index_sequence_helper<SequenceSizeValue>::type;

// meta
template <typename, typename enable = void> struct is_complete : std::false_type {};
template <typename Type> struct is_complete<Type, void_t<decltype(sizeof(Type))>> : std::true_type {};

template <typename FromType, typename ToType, typename enable = void>
struct is_static_castable : std::false_type {};

template <typename FromType, typename ToType>
struct is_static_castable<FromType, ToType, void_t<decltype( static_cast<ToType>(std::declval<FromType>()) )>> : std::true_type {};

template <typename PointerType, bool = std::is_pointer<PointerType>::value>
struct pointer_count
{
    static constexpr auto value = pointer_count<typename remove_pointer<PointerType>::type>::value + 1;
};

template <typename PointerType>
struct pointer_count<PointerType, false>
{
    static constexpr auto value = std::size_t(0);
};

struct shared_common_t {};
struct raw_common_t {};

struct dummy_t
{
#if __cplusplus >= 201703L
    template <typename Type, SF_REQUIRES(negation<std::is_same<Type, std::any>>::value)> operator Type();
#else
    template <typename Type> operator Type();
#endif // if
};

#if __cplusplus >= 201703L
template <class AggregateType, typename SequenceType = index_sequence<>, typename overload = void>
struct aggregate_size : SequenceType {};

template <class AggregateType, std::size_t... FieldIndexValues>
struct aggregate_size<AggregateType, index_sequence<FieldIndexValues...>,
                      void_t<decltype(AggregateType{ declval<dummy_t>(), declval<dummy_t, FieldIndexValues>()... })>>
    : aggregate_size<AggregateType, index_sequence<FieldIndexValues..., sizeof...(FieldIndexValues)>> {};
#endif // if

template <class, typename enable = void>
struct object_value { using type = dummy_t; };

template <class SerializableObjectType>
struct object_value<SerializableObjectType, void_t<typename SerializableObjectType::value_type>>
{
    using type = typename SerializableObjectType::value_type;
};

template <typename> struct array_value { using type = dummy_t; };
template <typename ValueType> struct array_value<ValueType[]> { using type = ValueType; };
template <typename ValueType, std::size_t SizeValue> struct array_value<ValueType[SizeValue]> { using type = ValueType; };

template <typename, typename enable = void>
struct value { using type = dummy_t; };

template <class SerializableObjectType>
struct value<SerializableObjectType, typename std::enable_if<std::is_class<SerializableObjectType>::value>::type>
{
    using type = typename object_value<SerializableObjectType>::type;
};

template <typename SerializableArrayType>
struct value<SerializableArrayType, typename std::enable_if<std::is_array<SerializableArrayType>::value>::type>
{
    using type = typename array_value<SerializableArrayType>::type;
};

template <typename FromType, typename ToType> struct is_cast_allowed
    : one<is_static_castable<FromType, ToType>, std::is_convertible<FromType, ToType>> {};

template <typename SerializableType> struct is_compressible : std::is_arithmetic<typename value<SerializableType>::type> {};

template <class DerivedType, class BaseType, class... BaseTypes> struct is_derived_of
    : all<std::is_base_of<BaseType, DerivedType>,
          std::is_base_of<BaseTypes, DerivedType>...> {};

template <class BaseType, class DerivedType> struct is_virtual_base_of
    : all<std::is_base_of<BaseType, DerivedType>,
          negation<is_static_castable<BaseType*, DerivedType*>>> {};

template <typename> struct is_std_shared_ptr : std::false_type {};
template <typename ElementType>
struct is_std_shared_ptr<std::shared_ptr<ElementType>> : std::true_type {};

template <typename> struct is_std_array : std::false_type {};
template <typename ValueType, std::size_t SizeValue>
struct is_std_array<std::array<ValueType, SizeValue>> : std::true_type {};

template <typename PointerType> struct is_shared_pointer : is_std_shared_ptr<PointerType> {};
template <typename PointerType> struct is_raw_pointer : std::is_pointer<PointerType> {};

template <typename PointerType> struct is_pointer : one<is_shared_pointer<PointerType>, is_raw_pointer<PointerType>> {};

template <typename PointerType> struct is_pointer_to_polymorphic
    : all<is_pointer<PointerType>, std::is_polymorphic<typename dereference<PointerType>::type>> {};

template <typename PointerType> struct is_void_pointer
    : all<is_pointer<PointerType>, std::is_void<typename dereference<PointerType>::type>> {};

template <typename PointerType> struct is_null_pointer : std::is_same<PointerType, std::nullptr_t> {};

template <typename> struct is_function_pointer : std::false_type {};
template <typename ReturnType, typename... ArgumentTypes>
struct is_function_pointer<ReturnType (*)(ArgumentTypes...)> : std::true_type {};

template <typename PointerType> struct is_pointer_to_standard_layout :
    all<is_pointer<PointerType>,
        negation<is_void_pointer<PointerType>>,
        negation<is_pointer_to_polymorphic<PointerType>>,
        negation<is_function_pointer<PointerType>>,
        negation<is_null_pointer<PointerType>>,
        negation<std::is_member_pointer<PointerType>>> {};

template <typename PointerType> struct is_serializable_pointer
    : one<is_pointer_to_standard_layout<PointerType>, is_pointer_to_polymorphic<PointerType>> {};

template <typename PointerType> struct is_serializable_raw_pointer
    : all<is_raw_pointer<PointerType>, is_serializable_pointer<PointerType>> {};

template <typename PointerType> struct is_serializable_shared_pointer
    : all<is_shared_pointer<PointerType>, is_serializable_pointer<PointerType>> {};

#if __cplusplus >= 201703L
template <typename AggregateType> struct is_aggregate
    : all<std::is_aggregate<AggregateType>,
          negation<is_std_array<AggregateType>>,
          negation<std::is_array<AggregateType>>> {};
#endif // if

template <typename SerializableType> struct is_unsupported :
    one<is_void_pointer<SerializableType>,
        is_function_pointer<SerializableType>,
        is_null_pointer<SerializableType>,
        std::is_function<SerializableType>,
        std::is_member_function_pointer<SerializableType>,
        std::is_member_object_pointer<SerializableType>,
        std::is_reference<SerializableType>> {};

} // namespace meta

} // namespace sf

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

    ::xxsf_archive_traits_key_type const trait;
    bool const readonly;
};

struct iarchive_common_t {};
struct oarchive_common_t {};

} // namespace core

namespace meta
{

template <class ArchiveType> struct is_iarchive : std::is_base_of<core::iarchive_common_t, ArchiveType> {};
template <class ArchiveType> struct is_oarchive : std::is_base_of<core::oarchive_common_t, ArchiveType> {};

template <class ArchiveType> struct is_ioarchive : one<is_iarchive<ArchiveType>, is_oarchive<ArchiveType>> {};

} // namespace meta

} // namespace sf

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
    template <typename S> \
    struct xxsf_##mode<S, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type> { \
        SF_DEPAREN(archive_template_header) \
        xxsf_##mode(SF_DEPAREN(archive_type)&, S&); \
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
    template <typename S> \
    SF_DEPAREN(archive_template_header) \
    xxsf_##mode<S, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type>::xxsf_##mode(SF_DEPAREN(archive_type)& archive, S& object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_save(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_load(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_saveload(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<S>(archive, object); } \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<S>(archive, object); }\
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(saveload, object_type_condition, archive_template_header, archive_type, object)
// ~Impl

// should be in global namespace
template <class SerializableType, typename enable = void> struct xxsf_save;
template <class SerializableType, typename enable = void> struct xxsf_load;
template <class SerializableType, typename enable = void> struct xxsf_saveload;

struct xxsf_cast_to_non_public_base
{
    template <class BaseType, class DerivedType>
    static BaseType& call(DerivedType& object)
    {
        return static_cast<BaseType&>(object);
    }
};

#if !defined(SF_TYPE_HASH) || !defined(SF_EXPRESSION_HASH)
#include <typeinfo> // type_info
#endif // if

#include <functional> // hash

#ifndef SF_STRING_HASH
    #define SF_STRING_HASH(string) ::sf::static_hash<::sf::let::u64>((string))
#endif // SF_STRING_HASH

#ifndef SF_TYPE_HASH
    #define SF_TYPE_HASH(type) (typeid(type).hash_code())
#endif // SF_TYPE_HASH

#ifndef SF_EXPRESSION_HASH
    #define SF_EXPRESSION_HASH(expression) (typeid(expression).hash_code())
#endif // SF_EXPRESSION_HASH

namespace sf
{

namespace detail
{

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
constexpr HashType static_fnv_1a(char const* text, HashType hash = OffsetBasis) noexcept
{
    return (*text == '\0')
           ? hash
           : static_fnv_1a<HashType, FnvPrime, OffsetBasis>(text + 1, (hash ^ (*text)) * FnvPrime);
}

template <typename HashType>
struct hash_t;

template <>
struct hash_t<let::u32>
{
private:
    static constexpr let::u32 fnv_prime = 16777619ull;
    static constexpr let::u32 fnv_offset_basis = 2166136261ull;

public:
    static let::u32 run(char const* text)
    {
        return detail::fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }

    static constexpr let::u32 static_run(char const* text) noexcept
    {
        return detail::static_fnv_1a<let::u32, fnv_prime, fnv_offset_basis>(text);
    }
};

template <>
struct hash_t<let::u64>
{
private:
    static constexpr let::u64 fnv_prime = 1099511628211ull;
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

template <typename HashType = let::u64>
HashType hash(const char* text)
{
    return detail::hash_t<HashType>::run(text);
}

template <typename HashType = let::u64>
constexpr HashType static_hash(char const* text) noexcept
{
    return detail::hash_t<HashType>::static_run(text);
}

namespace detail
{

template <typename HashType, typename ObjectType>
void hash_combine(HashType& seed, ObjectType const& object) noexcept
{
    auto hash = std::hash<ObjectType>{}(object) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= static_cast<HashType>(hash);
}

} // namespace detail

} // namepace sf

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

template <typename TraitsType> struct is_memory_shared : std::is_same<TraitsType, memory::shared_t> {};
template <typename TraitsType> struct is_memory_raw : std::is_same<TraitsType, memory::raw_t> {};

template <typename TraitsType> struct is_memory : one<is_memory_shared<TraitsType>, is_memory_raw<TraitsType>> {};

} // namespace meta

namespace memory
{

template <typename Type>
using shared_ptr = std::shared_ptr<Type>;

template <typename Type>
using raw_ptr = Type*;

template <typename PointerType>
struct ptr_traits
{
    using item = std::nullptr_t;
};

template <typename Type>
struct ptr_traits<std::shared_ptr<Type>>
{
    using traits = shared_t;

    template <typename OtherType>
    using wrapper = shared_ptr<OtherType>;

    using type = std::shared_ptr<Type>;
    using item = Type;

    using void_ptr = std::shared_ptr<void>;
};

template <typename Type>
struct ptr_traits<Type*>
{
    using traits = raw_t;

    template <typename OtherType>
    using wrapper = raw_ptr<OtherType>;

    using type = Type*;
    using item = Type;

    using void_ptr = void*;
};

template <typename Type>
struct ptr_traits<std::weak_ptr<Type>>
{
    using type = std::weak_ptr<Type>;
    using item = Type;
};

template <typename Type>
struct ptr_traits<std::unique_ptr<Type>>
{
    using type = std::unique_ptr<Type>;
    using item = Type;
};

template <typename Type>
struct factory_t
{
    static std::shared_ptr<Type> shared()
    {
        return std::make_shared<Type>();
    }

    static Type* raw()
    {
        return new Type{};
    }
};

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::is_raw_pointer<PointerType>::value)>
raw_ptr<ToType> dynamic_pointer_cast(PointerType const& pointer)
{
    return dynamic_cast<raw_ptr<ToType>>(pointer);
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::is_shared_pointer<PointerType>::value)>
shared_ptr<ToType> dynamic_pointer_cast(PointerType const& pointer)
{
    auto address = memory::dynamic_pointer_cast<ToType>(pointer.get());
    return address == nullptr ? shared_ptr<ToType>() : shared_ptr<ToType>(pointer, address);
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::one<meta::is_null_pointer<PointerType>,
                                meta::all<meta::is_pointer<PointerType>,
                                          meta::negation<meta::is_static_castable<typename ptr_traits<PointerType>::item*, ToType*>>>>::value)>
typename ptr_traits<PointerType>::template wrapper<ToType> static_pointer_cast(PointerType const&) noexcept
{
    return nullptr;
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_raw_pointer<PointerType>,
                                meta::is_static_castable<typename ptr_traits<PointerType>::item*, ToType*>>::value)>
raw_ptr<ToType> static_pointer_cast(PointerType const& pointer) noexcept
{
    return static_cast<raw_ptr<ToType>>(pointer);
}

template <typename ToType, typename FromType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::negation<meta::is_static_castable<FromType*, ToType*>>>::value)>
typename ptr_traits<PointerType>::template wrapper<ToType> static_pointer_cast(PointerType const&) noexcept
{
    return nullptr;
}

template <typename ToType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_shared_pointer<PointerType>,
                                meta::is_static_castable<typename ptr_traits<PointerType>::item*, ToType*>>::value)>
shared_ptr<ToType> static_pointer_cast(PointerType const& pointer) noexcept
{
    auto address = memory::static_pointer_cast<ToType>(pointer.get());
    return shared_ptr<ToType>(pointer, address);
}

template <typename ToType, typename FromType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::is_static_castable<typename ptr_traits<PointerType>::item*, FromType*>,
                                meta::is_static_castable<FromType*, ToType*>>::value)>
typename ptr_traits<PointerType>::template wrapper<ToType> static_pointer_cast(PointerType const& pointer) noexcept
{
    return memory::static_pointer_cast<ToType>(memory::static_pointer_cast<FromType>(pointer));
}

template <typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
typename ptr_traits<PointerType>::void_ptr pure(PointerType const& pointer) noexcept
{
    return memory::static_pointer_cast<void>(pointer);
}

template <typename PointerType,
          SF_REQUIRES(meta::all<meta::is_pointer<PointerType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
typename ptr_traits<PointerType>::void_ptr pure(PointerType const& pointer_to_polymorphic)
{
    return memory::dynamic_pointer_cast<void>(pointer_to_polymorphic);
}

inline raw_ptr<void> pure(std::nullptr_t) noexcept { return nullptr; }

template <typename Type, typename PointerType,
          SF_REQUIRES(meta::is_pointer<PointerType>::value)>
void assign(PointerType& pointer, typename ptr_traits<PointerType>::void_ptr const& address) noexcept
{
    pointer = memory::static_pointer_cast<Type>(address);
}

template <typename ToType, typename FromType = ToType, typename TraitsType,
          SF_REQUIRES(meta::all<meta::is_memory<TraitsType>,
                                meta::one<meta::negation<meta::is_static_castable<FromType*, ToType*>>,
                                          std::is_abstract<FromType>>>::value)>
std::nullptr_t allocate() noexcept
{
    return nullptr;
}

template <typename ToType, typename FromType = ToType, typename TraitsType,
          SF_REQUIRES(meta::all<meta::negation<std::is_abstract<FromType>>,
                                meta::is_memory_shared<TraitsType>,
                                meta::is_static_castable<FromType*, ToType*>>::value)>
shared_ptr<ToType> allocate()
{
    auto instance = factory_t<FromType>::shared();
    return memory::static_pointer_cast<ToType>(instance);
}

template <typename ToType, typename FromType = ToType, typename TraitsType,
          SF_REQUIRES(meta::all<meta::negation<std::is_abstract<FromType>>,
                                meta::is_memory_raw<TraitsType>,
                                meta::is_static_castable<FromType*, ToType*>>::value)>
raw_ptr<ToType> allocate()
{
    auto instance = factory_t<FromType>::raw();
    return memory::static_pointer_cast<ToType>(instance);
}

template <typename ToType, typename FromType = ToType>
shared_ptr<ToType> allocate_shared()
{
    return memory::allocate<ToType, FromType, shared_t>();
}

template <typename ToType, typename FromType = ToType>
raw_ptr<ToType> allocate_raw()
{
    return memory::allocate<ToType, FromType, raw_t>();
}

template <typename ToType, typename FromType = ToType, typename PointerType,
          SF_REQUIRES(meta::is_pointer<PointerType>::value)>
void allocate(PointerType& pointer)
{
    pointer = memory::allocate<ToType, FromType, typename ptr_traits<PointerType>::traits>();
}

template <typename PointerType,
          SF_REQUIRES(meta::is_raw_pointer<PointerType>::value)>
raw_ptr<typename ptr_traits<PointerType>::item> raw(PointerType const& pointer) { return pointer; }

template <typename PointerType,
          SF_REQUIRES(meta::is_shared_pointer<PointerType>::value)>
raw_ptr<typename ptr_traits<PointerType>::item> raw(PointerType const& pointer) { return pointer.get(); }

template <typename ByteType = char, typename DataType>
ByteType const* const_byte_cast(DataType* data) noexcept
{
    return reinterpret_cast<ByteType const*>(data);
}

template <typename ByteType = char, typename DataType>
ByteType* byte_cast(DataType* data) noexcept
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
    template <typename SerializableType>
    static void save(ioarchive_t& archive, SerializableType& data)
    {
        call<::xxsf_oarchive_registry>(archive, data);
    }

    template <typename SerializableType>
    static void load(ioarchive_t& archive, SerializableType& data)
    {
        call<::xxsf_iarchive_registry>(archive, data);
    }

private:
    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKeyValue,
              typename SerializableType,
              SF_REQUIRES(ArchiveKeyValue == ::xxsf_archive_traits_base_key)>
    static void call(ioarchive_t&, SerializableType&)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKeyValue = 0,
              typename SerializableType,
              SF_REQUIRES(ArchiveKeyValue < ::xxsf_archive_traits_base_key)>
    static void call(core::ioarchive_t& archive, SerializableType& data)
    {
        using DerivedArchiveType = typename ArchiveRegistryTemplate<ArchiveKeyValue>::type;

        if (::xxsf_archive_traits<DerivedArchiveType>::key == archive.trait)
            return try_call<DerivedArchiveType>(archive, data);

        call<ArchiveRegistryTemplate, ArchiveKeyValue + 1>(archive, data);
    }

    template <class DerivedArchiveType, typename SerializableType,
              SF_REQUIRES(::xxsf_archive_traits<DerivedArchiveType>::key == ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t&, SerializableType&)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchiveType, typename SerializableType,
              SF_REQUIRES(::xxsf_archive_traits<DerivedArchiveType>::key != ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t& archive, SerializableType& data)
    {
    #ifdef SF_DEBUG
        if (typeid(archive) != typeid(DerivedArchiveType))
            throw "The read/write archive was registered incorrect.";
    #endif // SF_DEBUG
        try_call_impl<DerivedArchiveType>(static_cast<DerivedArchiveType&>(archive), data);
    }

    template <class DerivedArchiveType, typename SerializableType>
    static void try_call_impl(iarchive_common_t& archive, SerializableType& data)
    {
        ::xxsf_load<SerializableType>(static_cast<DerivedArchiveType&>(archive), data);
    }

    template <class DerivedArchiveType, typename SerializableType>
    static void try_call_impl(oarchive_common_t& archive, SerializableType& data)
    {
        ::xxsf_save<SerializableType>(static_cast<DerivedArchiveType&>(archive), data);
    }
};

template <typename SerializableType,
          SF_REQUIRES(meta::is_unsupported<SerializableType>::value)>
ioarchive_t& operator& (ioarchive_t& archive, SerializableType const&)
{
    static_assert(meta::to_false<SerializableType>(),
        "The 'SerializableType' is an unsupported type for the 'sf::ioarchive_t'.");

    return archive;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator<< (ioarchive_t& archive, SerializableType const& data)
{
    polymorphic_archive_t::save(archive, const_cast<SerializableType&>(data));
    return archive;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator>> (ioarchive_t& archive, SerializableType const& data)
{
    polymorphic_archive_t::load(archive, const_cast<SerializableType&>(data));
    return archive;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator& (ioarchive_t& archive, SerializableType const& data)
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

#define EXPORT_INSTANTIABLE(...)                                                                        \
    template <> struct xxsf_instantiable_fixture<__VA_ARGS__> { static bool fixture; };                 \
    bool xxsf_instantiable_fixture<__VA_ARGS__>::fixture =                                              \
        ::sf::dynamic::instantiable_registry_t::instance().fixture<__VA_ARGS__>();

#define EXPORT_INSTANTIABLE_KEY(instantiable_key, ...)                                                  \
    template <> struct xxsf_instantiable_traits<__VA_ARGS__> {                                          \
        static ::xxsf_instantiable_traits_key_type key() { return instantiable_key; }                   \
    };                                                                                                  \
    EXPORT_INSTANTIABLE(__VA_ARGS__)

using xxsf_instantiable_traits_key_type = ::sf::let::u64;
static constexpr auto xxsf_instantiable_traits_base_key = xxsf_instantiable_traits_key_type(-1);

template <class InstantiableType>
struct xxsf_instantiable_traits
{
    static ::xxsf_instantiable_traits_key_type key() { return SF_TYPE_HASH(InstantiableType); }
};

template <class InstantiableType>
struct xxsf_instantiable_fixture;

namespace sf
{

namespace dynamic
{

class instantiable_registry_t
{
public:
    using instantiable_type = INSTANTIABLE_TYPE;

public:
    struct instantiable_proxy_t
    {
        ::xxsf_instantiable_traits_key_type key = ::xxsf_archive_traits_base_key;

        std::shared_ptr<instantiable_type>(*shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*cast_shared)(std::shared_ptr<void> const&) = nullptr;

        instantiable_type*(*raw)() = nullptr;
        instantiable_type*(*cast_raw)(void*) = nullptr;

        void(*save)(core::ioarchive_t&, instantiable_type*) = nullptr;
        void(*load)(core::ioarchive_t&, instantiable_type*) = nullptr;
    };

public:
    std::unordered_map<::xxsf_instantiable_traits_key_type, instantiable_proxy_t> all;
    std::unordered_map<let::u64, instantiable_proxy_t> rtti_all;

private:
    instantiable_registry_t() = default;
    instantiable_registry_t(instantiable_registry_t const&) = delete;
    instantiable_registry_t& operator=(instantiable_registry_t const&) = delete;

public:
    static instantiable_registry_t& instance() noexcept
    {
        static instantiable_registry_t self;
        return self;
    }

public:
    template <typename InstantiableType> struct is_instantiable
        : meta::is_cast_allowed<InstantiableType*, instantiable_type*> {};

public:
    template <class InstantiableType, SF_REQUIRES(meta::negation<is_instantiable<InstantiableType>>::value)>
    void add() { /*pass*/ }

    template <class InstantiableType, SF_REQUIRES(is_instantiable<InstantiableType>::value)>
    void add()
    {
        static bool lock = false; if (lock) return;
        lock = true;

        auto const key = ::xxsf_instantiable_traits<InstantiableType>::key();
    #ifdef SF_DEBUG
        if (key == ::xxsf_instantiable_traits_base_key)
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with valid key.";
    #endif // SF_DEBUG
        instantiable_proxy_t proxy;

        proxy.key = key;

        proxy.shared = [] { return memory::allocate_shared<instantiable_type, InstantiableType>(); };

        proxy.cast_shared = [](std::shared_ptr<void> const& address)
        {
            return memory::static_pointer_cast<instantiable_type, InstantiableType>(address);
        };

        proxy.raw = [] { return memory::allocate_raw<instantiable_type, InstantiableType>(); };

        proxy.cast_raw = [](void* address)
        {
            return memory::static_pointer_cast<instantiable_type, InstantiableType>(address);
        };

        proxy.save = [](core::ioarchive_t& archive, instantiable_type* instance)
        {
            archive << *memory::dynamic_pointer_cast<InstantiableType>(instance);
        };

        proxy.load = [](core::ioarchive_t& archive, instantiable_type* instance)
        {
            archive >> *memory::dynamic_pointer_cast<InstantiableType>(instance);
        };

        all.emplace(key, proxy);

        auto const hash = SF_TYPE_HASH(InstantiableType);
        rtti_all.emplace(hash, proxy);
    }

    template <class InstantiableType>
    bool fixture()
    {
    #ifdef SF_DEBUG
        if (meta::negation<is_instantiable<InstantiableType>>::value)
            throw "The polymorphic 'InstantiableType' type is not convertible to 'instantiable_t'.";
    #endif // SF_DEBUG

        if (all.find(::xxsf_instantiable_traits<InstantiableType>::key()) != all.end())
        #ifdef SF_DEBUG
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with unique key.";
        #else
            return false;
        #endif // SF_DEBUG

        add<InstantiableType>();
        return true;
    }

    template <typename PointerTraitsType,
              SF_REQUIRES(meta::is_memory_shared<PointerTraitsType>::value)>
    std::shared_ptr<instantiable_type> clone(::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key).shared();
    }

    template <typename PointerTraitsType,
              SF_REQUIRES(meta::is_memory_raw<PointerTraitsType>::value)>
    instantiable_type* clone(::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key).raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> const& address, ::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key).cast_shared(address);
    }

    instantiable_type* cast(void* address, ::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key).cast_raw(address);
    }

    template <typename PointerType>
    void save(core::ioarchive_t& archive, PointerType& pointer) const
    {
        auto const hash = SF_EXPRESSION_HASH(*pointer);
        rtti_all.at(hash).save(archive, pointer);
    }

    template <typename PointerType>
    void load(core::ioarchive_t& archive, PointerType& pointer) const
    {
        auto const hash = SF_EXPRESSION_HASH(*pointer);
        rtti_all.at(hash).load(archive, pointer);
    }
};

} // namespace dynamic

} // namespace sf

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

namespace sf
{

namespace dynamic
{

class any_registry_t
{
public:
    struct any_proxy_t
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*save)(core::ioarchive_t&, std::any&) = nullptr;
        void(*load)(core::ioarchive_t&, std::any&) = nullptr;
    };

public:
    std::unordered_map<let::u64, any_proxy_t> all;

private:
    any_registry_t() = default;
    any_registry_t(any_registry_t const&) = delete;
    any_registry_t& operator=(any_registry_t const&) = delete;

public:
    static any_registry_t& instance() noexcept
    {
        static any_registry_t self;
        return self;
    }

    template <typename SerializableType>
    void add()
    {
        static bool lock = false; if (lock) return;
        lock = true;

        any_proxy_t proxy;

        proxy.save = [](core::ioarchive_t& archive, std::any& any)
        {
            archive << std::any_cast<SerializableType&>(any);
        };

        proxy.load = [](core::ioarchive_t& archive, std::any& any)
        {
            any.emplace<SerializableType>();
            archive >> std::any_cast<SerializableType&>(any);
        };

        all.emplace(typeid(SerializableType).hash_code(), proxy);
    }

public:
    void save(core::ioarchive_t& archive, std::any& any, let::u64 hash)
    {
        all.at(hash).save(archive, any);
    }

    void load(core::ioarchive_t& archive, std::any& any, let::u64 hash)
    {
        all.at(hash).load(archive, any);
    }
};

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
    template <class ArchiveType, typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static ::xxsf_instantiable_traits_key_type save_key(ArchiveType& archive, PointerType& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto const hash = SF_EXPRESSION_HASH(*pointer);

        auto key = instantiable_registry_t::instance().rtti_all.at(hash).key;
        archive & key;

        return key;
    }

    template <class ArchiveType, typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static ::xxsf_instantiable_traits_key_type load_key(ArchiveType& archive, PointerType& pointer)
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
    template <typename PointerType> struct is_pointer_to_instantiable
        : meta::all<instantiable_registry_t::is_instantiable<typename memory::ptr_traits<PointerType>::item>,
                    meta::is_pointer_to_polymorphic<PointerType>> {};

public:
    template <typename PointerType,
              SF_REQUIRES(is_pointer_to_instantiable<PointerType>::value)>
    static void save(core::ioarchive_t& archive, PointerType& pointer, ::xxsf_instantiable_traits_key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = memory::raw(pointer);
        instantiable_registry_t::instance().save(archive, raw_pointer);
    }

    template <typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static void load(core::ioarchive_t& archive, PointerType& pointer,
                     ::xxsf_instantiable_traits_key_type key, typename memory::ptr_traits<PointerType>::void_ptr& cache)
    {
        using PointerTraitsType = memory::ptr_traits<PointerType>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto& registry = instantiable_registry_t::instance();

        auto cloned = registry.clone<typename PointerTraitsType::traits>(key);

        pointer = memory::dynamic_pointer_cast<typename PointerTraitsType::item>(cloned);
        cache = memory::pure(pointer);

        auto raw_pointer = memory::raw(pointer);
        registry.load(archive, raw_pointer);
    }

    template <typename PointerType,
              SF_REQUIRES(meta::is_pointer_to_polymorphic<PointerType>::value)>
    static void assign(PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr const& address,
                       ::xxsf_instantiable_traits_key_type key)
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto casted = instantiable_registry_t::instance().cast(address, key);
        pointer = memory::dynamic_pointer_cast<typename memory::ptr_traits<PointerType>::item>(casted);
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

template <typename PointerType>
struct track_traits;

template <typename Type>
struct track_traits<memory::shared_ptr<Type>>
{
    using type = tracking::shared_t;
};

template <typename Type>
struct track_traits<memory::raw_ptr<Type>>
{
    using type = tracking::raw_t;
};

template <typename TrackType>
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

template <typename TrackType> struct is_track_shared : std::is_same<TrackType, tracking::shared_t> {};
template <typename TrackType> struct is_track_raw : std::is_same<TrackType, tracking::raw_t> {};

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

template <typename TrackType> struct is_track_hierarchy : std::is_same<TrackType, tracking::hierarchy_t> {};

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

template <typename OutputStreamType = std::vector<unsigned char>>
class obyte_stream_t
{
protected:
    using item_type = typename OutputStreamType::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::obyte_stream_t'.");

public:
    OutputStreamType& storage;

public:
    obyte_stream_t(OutputStreamType& stream) noexcept : storage(stream)
    {
        storage.reserve(SF_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename CharType>
    void call(CharType const* data, std::size_t size)
    {
        auto it = memory::const_byte_cast<item_type>(data);
        while (size > 0)
        {
            storage.emplace_back(*it++);
            --size;
        }
    }
};

template <typename InputStreamType = std::vector<unsigned char>>
struct ibyte_stream_t
{
protected:
    using item_type = typename InputStreamType::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::ibyte_stream_t'.");

public:
    InputStreamType& storage;
    std::size_t offset;

    ibyte_stream_t(InputStreamType& stream) noexcept : storage(stream), offset() {}

    template <typename CharType>
    void call(CharType* data, std::size_t size)
    {
        auto it = memory::byte_cast<item_type>(data);
        while (size > 0)
        {
            *it++ = storage[offset++];
            --size;
        }
    }
};

template <typename OutputStreamType = std::ofstream>
class ofile_stream_t
{
public:
    OutputStreamType& file;

public:
    ofile_stream_t(OutputStreamType& stream) noexcept : file(stream) {}

    template <typename CharType>
    void call(CharType const* data, std::size_t memory_size)
    {
        file.write(memory::const_byte_cast(data), memory_size);
    }
};

template <typename InputStreamType = std::ifstream>
class ifile_stream_t
{
public:
    InputStreamType& file;

public:
    ifile_stream_t(InputStreamType& stream) noexcept : file(stream) {}

    template <typename CharType>
    void call(CharType* data, std::size_t memory_size)
    {
        file.read(memory::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace sf

namespace sf
{

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t>
class oarchive_t : public core::ioarchive_t, public core::oarchive_common_t
{
public:
    using TrackingKeyType = std::uintptr_t;

private:
    StreamWrapperType xxarchive;

    std::unordered_map<TrackingKeyType, bool> xxtrack_shared;
    std::unordered_map<TrackingKeyType, bool> xxtrack_raw;

    tracking::hierarchy_track_t<std::uintptr_t> xxtrack_hierarchy;

    RegistryType xxregistry;

public:
    template <typename OutputStreamType>
    oarchive_t(OutputStreamType& stream) : core::ioarchive_t(::xxsf_archive_traits<oarchive_t>::key, false)
        , xxarchive{stream}, xxtrack_shared(), xxtrack_raw(), xxtrack_hierarchy(), xxregistry() {}

    StreamWrapperType& stream() noexcept { return xxarchive; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_shared)& { return xxtrack_shared; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_raw)& { return xxtrack_raw; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_hierarchy)& { return xxtrack_hierarchy; }

    RegistryType& registry() noexcept { return xxregistry; }

    template <typename SerializableType>
    oarchive_t& operator<< (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType>
    oarchive_t& operator& (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType, typename... SerializableTypes>
    oarchive_t& operator() (SerializableType const& data, SerializableTypes const&... datas)
    {
        ::xxsf_save<SerializableType>(*this, const_cast<SerializableType&>(data));
        return operator()(datas...);
    }

    oarchive_t& operator() () noexcept { return *this; }
};

// create default oarchive_t with wrapper::obyte_stream_t<>
template <typename OutputStreamType>
oarchive_t<wrapper::obyte_stream_t<OutputStreamType>> oarchive(OutputStreamType& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapperTemplate,
          class RegistryType = dynamic::extern_registry_t,
          typename OutputStreamType>
oarchive_t<StreamWrapperTemplate<OutputStreamType>, RegistryType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t,
          typename OutputStreamType>
oarchive_t<StreamWrapperType, RegistryType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

} // namespace sf

namespace sf
{

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t>
class iarchive_t : public core::ioarchive_t, public core::iarchive_common_t
{
public:
    using TrackingKeyType = std::uintptr_t;

private:
    StreamWrapperType xxarchive;

    std::unordered_map<TrackingKeyType, memory::shared_ptr<void>> xxtrack_shared;
    std::unordered_map<TrackingKeyType, memory::raw_ptr<void>> xxtrack_raw;

    tracking::hierarchy_track_t<TrackingKeyType> xxtrack_hierarchy;

    RegistryType xxregistry;

public:
    template <typename InputStreamType>
    iarchive_t(InputStreamType& stream) : core::ioarchive_t(::xxsf_archive_traits<iarchive_t>::key, true)
        , xxarchive{stream}, xxtrack_shared(), xxtrack_raw(), xxtrack_hierarchy(), xxregistry() {}

    StreamWrapperType& stream() noexcept { return xxarchive; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_shared)& { return xxtrack_shared; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_raw)& { return xxtrack_raw; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_hierarchy)& { return xxtrack_hierarchy; }

    RegistryType& registry() noexcept { return xxregistry; }

    template <typename SerializableType>
    iarchive_t& operator>> (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType>
    iarchive_t& operator& (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType, typename... SerializableTypes>
    iarchive_t& operator() (SerializableType const& data, SerializableTypes const&... datas)
    {
        ::xxsf_load<SerializableType>(*this, const_cast<SerializableType&>(data));
        return operator()(datas...);
    }

    iarchive_t& operator() () { return *this; }
};

// create default iarchive_t with wrapper::ibyte_stream_t<>
template <typename InputStreamType>
iarchive_t<wrapper::ibyte_stream_t<InputStreamType>> iarchive(InputStreamType& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapperTemplate,
          class RegistryType = dynamic::extern_registry_t,
          typename InputStreamType>
iarchive_t<StreamWrapperTemplate<InputStreamType>, RegistryType> iarchive(InputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t,
          typename InputStreamType>
iarchive_t<StreamWrapperType, RegistryType> iarchive(InputStreamType& stream)
{
    return { stream };
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

template <typename ApplyFunctorType> struct is_apply_functor : std::is_base_of<apply::apply_functor_t, ApplyFunctorType> {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, apply_functor, ::sf::meta::is_apply_functor<typename std::decay<S>::type>::value)
{
    apply_functor(archive);
}

namespace sf
{

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
void binary(ArchiveType& archive, SerializableType& data)
{
    archive.stream().call(std::addressof(data), sizeof(SerializableType));
}

namespace apply
{

template <typename SerializableType>
struct binary_functor_t : apply_functor_t
{
    SerializableType& data;

    binary_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { binary(archive, data); }
};

} // namespace apply

template <typename SerializableType>
apply::binary_functor_t<SerializableType> binary(SerializableType& object) noexcept { return { object }; }

} // namespace sf

namespace sf
{

namespace detail
{

template <class ArchiveType, typename PointerType, typename KeyType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void native_save(ArchiveType&, PointerType&, KeyType) noexcept { /*pass*/ }

template <class ArchiveType, typename PointerType, typename KeyType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void native_save(ArchiveType& archive, PointerType& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void native_load(ArchiveType&, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& address) noexcept
{
    memory::assign<typename memory::ptr_traits<PointerType>::item>(pointer, address);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void native_load(ArchiveType& archive, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

namespace sf
{

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_standard_layout<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    if (pointer == nullptr)
        throw "The write pointer must be allocated.";

    archive & (*pointer);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_standard_layout<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& cache)
{
    using item_type = typename memory::ptr_traits<PointerType>::item;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    memory::allocate<item_type>(pointer);
    cache = memory::pure(pointer);

    archive & (*pointer);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    auto& registry = archive.registry();

    auto const key = registry.save_key(archive, pointer);
    registry.save(archive, pointer, key);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& cache)
{
    auto& registry = archive.registry();

    auto const key = registry.load_key(archive, pointer);
    registry.load(archive, pointer, key, cache);
}

// verison without cache using
template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_serializable_pointer<PointerType>>::value)>
void strict(ArchiveType& archive, PointerType& pointer)
{
    typename memory::ptr_traits<PointerType>::void_ptr cache = nullptr;
    strict(archive, pointer, cache);
}

namespace detail
{

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_serializable_pointer<PointerType>>::value)>
typename ArchiveType::TrackingKeyType refer_key(ArchiveType& archive, PointerType& pointer)
{
    auto pure = memory::pure(pointer);
    auto key = reinterpret_cast<typename ArchiveType::TrackingKeyType>(memory::raw(pure));

    archive & key;
    return key;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_serializable_pointer<PointerType>>::value)>
typename ArchiveType::TrackingKeyType refer_key(ArchiveType& archive, PointerType& pointer)
{
#ifdef SF_DEBUG
    if (pointer != nullptr)
        throw "The read pointer must be initialized to nullptr.";
#endif // SF_DEBUG

    typename ArchiveType::TrackingKeyType key{};
    archive & key;

    return key;
}

} // namespace detail

namespace apply
{

template <typename SerializableType>
struct strict_functor_t : public apply_functor_t
{
    SerializableType& data;

    strict_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { strict(archive, data); }
};

} // namespace apply

template <typename SerializableType>
apply::strict_functor_t<SerializableType>
strict(SerializableType& parameter) noexcept { return { parameter }; }

} // namespace sf

namespace sf
{

namespace tracking
{

template <typename TrackType, class ArchiveType, typename KeyType,
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
bool is_track(ArchiveType& archive, KeyType key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class ArchiveType, typename KeyType,
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
bool is_mixed(ArchiveType& archive, KeyType key)
{
    using reverse_track_type = typename reverse_traits<TrackType>::type;
    return is_track<reverse_track_type>(archive, key);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer<PointerType>>::value)>
void track(ArchiveType& archive, PointerType& pointer)
{
    using track_type = typename tracking::track_traits<PointerType>::type;

    auto const key = detail::refer_key(archive, pointer); // serialize refer info
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

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::negation<meta::is_pointer<SerializableType>>>::value)>
void track(ArchiveType& archive, SerializableType& data)
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

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer<PointerType>>::value)>
void track(ArchiveType& archive, PointerType& pointer)
{
    using track_type = typename tracking::track_traits<PointerType>::type;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    auto const key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

    auto& address = archive.template tracking<track_type>()[key];

    if (address == nullptr)
    {
        // call the strict serialization of not tracking pointer
        strict(archive, pointer, address);
    }
    else
    {
        detail::native_load(archive, pointer, address);
    }
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::negation<meta::is_pointer<SerializableType>>>::value)>
void track(ArchiveType& archive, SerializableType& data)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    key_type key{};
    archive & key;

    auto& address = archive.template tracking<tracking::raw_t>()[key];

    if (address != nullptr)
        throw  "The read tracking data is already tracked.";

    address = memory::pure(std::addressof(data));

    archive & data;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<PointerType>,
                                meta::is_serializable_raw_pointer<PointerType>>::value)>
void raw(ArchiveType& archive, PointerType& pointer)
{
    if (detail::refer_key(archive, pointer)) // serialize refer info
        strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

template <typename SerializableType>
struct track_functor_t : apply_functor_t
{
    SerializableType& data;

    track_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { tracking::track(archive, data); }
};

template <typename SerializableType>
struct raw_functor_t : apply_functor_t
{
    SerializableType& data;

    raw_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { tracking::raw(archive, data); }
};

} // namespace apply

namespace tracking
{

template <typename SerializableType>
apply::track_functor_t<SerializableType> track(SerializableType& data) noexcept { return { data }; }

template <typename SerializableType>
apply::raw_functor_t<SerializableType> raw(SerializableType& data) noexcept { return { data }; }

} // namespace tracking

} // namespace sf

#include <valarray> // valarray

namespace sf
{

namespace utility
{

template <class ContainerType>
typename ContainerType::value_type const* data(ContainerType const& c) noexcept
{
    // if begin return iterator - we should deref it and take address again
    return std::addressof(*std::begin(c));
}

template <typename ValueType>
ValueType const* data(std::valarray<ValueType> const& c) noexcept
{
    return std::begin(c);
}

template <typename ValueType, std::size_t SizeValue>
ValueType const* data(ValueType const (&array)[SizeValue]) noexcept
{
    return array;
}

template <class ContainerType>
auto size(ContainerType const& c) noexcept -> decltype(c.size())
{
    return c.size();
}

template <typename ValueType, std::size_t SizeValue>
constexpr std::size_t size(ValueType const (&)[SizeValue]) noexcept
{
    return SizeValue;
}

} // namespace utility

} // namespace sf

namespace sf
{

namespace compress
{

// always require compressible type for fast compression
template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::is_compressible<SerializableType>>::value)>
void fast(ArchiveType& archive, SerializableType& object)
{
    using item_type = typename meta::value<SerializableType>::type;

    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
void slow(ArchiveType& archive, SerializableType& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::is_compressible<SerializableType>>::value)>
void zip(ArchiveType& archive, SerializableType& object)
{
    fast(archive, object);
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::negation<meta::is_compressible<SerializableType>>>::value)>
void zip(ArchiveType& archive, SerializableType& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, number, std::is_arithmetic<S>::value)
{
    ::sf::binary(archive, number);
}

CONDITIONAL_SERIALIZATION(save, enumerator, std::is_enum<S>::value)
{
    auto value = static_cast<typename std::underlying_type<S>::type>(enumerator);
    archive & value;
}

CONDITIONAL_SERIALIZATION(load, enumerator, std::is_enum<S>::value)
{
    typename std::underlying_type<S>::type buff{};
    archive & buff;

    enumerator = static_cast<S>(buff);
}

CONDITIONAL_SERIALIZATION(saveload, array, std::is_array<S>::value)
{
    ::sf::compress::zip(archive, array);
}

CONDITIONAL_SERIALIZATION(saveload, pointer, ::sf::meta::is_serializable_raw_pointer<S>::value)
{
#ifdef SF_PTRTRACK_DISABLE
    ::sf::tracking::raw(archive, pointer);
#else
    ::sf::tracking::track(archive, pointer);
#endif // SF_PTRTRACK_DISABLE
}

#if __cplusplus >= 201703L

#define SF_AGGREGATE_IMPLEMENTATION_GENERIC(count)                                                      \
    template <class ArchiveType, typename SerializableType>                                             \
    void aggregate_impl(ArchiveType& archive, SerializableType& object,                                 \
                        std::integral_constant<std::size_t, count>) {                                   \
        auto& [SF_PLACEHOLDERS(count)] = object;                                                        \
        archive(SF_PLACEHOLDERS(count));                                                                \
    }

namespace sf
{

namespace meta
{

template <typename SerializableType> struct is_serializable_aggregate
    : all<is_aggregate<SerializableType>, negation<std::is_union<SerializableType>>> {};

} // namespace meta

namespace detail
{

template <class ArchiveType, typename SerializableType>
void aggregate_impl(ArchiveType& archive, SerializableType& object, std::integral_constant<std::size_t, 0>) noexcept { /*pass*/ }

SF_REPEAT(SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::is_aggregate<SerializableType>>::value)>
void aggregate(ArchiveType& archive, SerializableType& object)
{
    constexpr auto size = meta::aggregate_size<SerializableType>::value;
    detail::aggregate_impl(archive, object, std::integral_constant<std::size_t, size>{});
}

namespace apply
{

template <typename SerializableType>
struct aggregate_functor_t : apply_functor_t
{
    SerializableType& object;

    aggregate_functor_t(SerializableType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { aggregate(archive, object); }
};

} // namespace apply

template <typename SerializableType>
apply::aggregate_functor_t<SerializableType> aggregate(SerializableType& object) noexcept { return { object }; }

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, object, ::sf::meta::is_serializable_aggregate<S>::value)
{
    ::sf::aggregate(archive, object);
}

// clean up
#undef SF_AGGREGATE_IMPLEMENTATION_GENERIC

#endif // if

CONDITIONAL_SERIALIZATION(saveload, data, std::is_union<S>::value)
{
    ::sf::binary(archive, data);
}

namespace sf
{

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                std::is_base_of<BaseType, DerivedType>>::value)>
void base(ArchiveType& archive, DerivedType& object)
{
    archive & ::xxsf_cast_to_non_public_base::call<BaseType>(object);
}

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                std::is_base_of<BaseType, DerivedType>>::value)>
void virtual_base(ArchiveType& archive, DerivedType& object)
{
#ifdef SF_PTRTRACK_DISABLE
    if (SF_EXPRESSION_HASH(object) == SF_TYPE_HASH(DerivedType))
        base<Base>(archive, object);
#else
    using key_type = typename ArchiveType::TrackingKeyType;

    auto address = memory::pure(std::addressof(object));

    auto const key = reinterpret_cast<key_type>(address);
    auto const traits = SF_TYPE_HASH(BaseType);

    auto& hierarchy_tracking = archive.template tracking<tracking::hierarchy_t>();

    auto& is_tracking = hierarchy_tracking[{key, traits}];
    if (not is_tracking)
    {
        is_tracking = true;
        base<BaseType>(archive, object);
    }
#endif // SF_PTRTRACK_DISABLE
}

namespace detail
{

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(meta::negation<meta::is_virtual_base_of<BaseType, DerivedType>>::value)>
void native_base(ArchiveType& archive, DerivedType& object_with_base)
{
    base<BaseType>(archive, object_with_base);
}

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(meta::is_virtual_base_of<BaseType, DerivedType>::value)>
void native_base(ArchiveType& archive, DerivedType& object_with_virtual_base)
{
    virtual_base<BaseType>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <class DerivedType, class BaseType>
struct base_functor_t : apply_functor_t
{
    DerivedType& object;

    base_functor_t(DerivedType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { base<BaseType>(archive, object); }
};

template <class DerivedType, class BaseType>
struct virtual_base_functor_t : apply_functor_t
{
    DerivedType& object;

    virtual_base_functor_t(DerivedType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { virtual_base<BaseType>(archive, object); }
};

} // namespace apply

template <class BaseType, class DerivedType,
          SF_REQUIRES(std::is_base_of<BaseType, DerivedType>::value)>
apply::base_functor_t<DerivedType, BaseType> base(DerivedType& object) noexcept { return { object }; }

template <class BaseType, class DerivedType,
          SF_REQUIRES(std::is_base_of<BaseType, DerivedType>::value)>
apply::virtual_base_functor_t<DerivedType, BaseType> virtual_base(DerivedType& object) noexcept { return { object }; }

// default empty impl
template <class ArchiveType, class DerivedType>
void hierarchy(ArchiveType&, DerivedType&) noexcept { /*pass*/ }

// Variadic native_base function
template <class BaseType, class... BaseTypes, class ArchiveType, class DerivedType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::is_derived_of<DerivedType, BaseType, BaseTypes...>>::value)>
void hierarchy(ArchiveType& archive, DerivedType& object)
{
    detail::native_base<BaseType>(archive, object);
    hierarchy<BaseTypes...>(archive, object);
}

namespace apply
{

template <class DerivedType, class BaseType, class... BaseTypes>
struct hierarchy_functor_t : apply_functor_t
{
    DerivedType& object;

    hierarchy_functor_t(DerivedType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { hierarchy<BaseType, BaseTypes...>(archive, object); }
};

} // namespace apply

template <class BaseType, class... BaseTypes, class DerivedType,
          SF_REQUIRES(meta::is_derived_of<DerivedType, BaseType, BaseTypes...>::value)>
apply::hierarchy_functor_t<DerivedType, BaseType, BaseTypes...> hierarchy(DerivedType& object) noexcept
{
    return { object };
}

} // namespace sf

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend struct ::xxsf_cast_to_non_public_base;                                                       \
    template <typename, typename> friend struct ::xxsf_save;                                            \
    template <typename, typename> friend struct ::xxsf_load;                                            \
    template <typename, typename> friend struct ::xxsf_saveload;

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename SerializableType> void serializable()
{
    static_assert(meta::negation<meta::is_unsupported<SerializableType>>::value,
        "The 'SerializableType' is an unsupported type for serialization.");

    dynamic::instantiable_registry_t::instance().add<SerializableType>();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::any_registry_t::instance().add<SerializableType>();
#endif // if
}

template <typename SerializableType> SerializableType&& serializable(SerializableType&& object)
{
    serializable<typename std::decay<SerializableType>::type>();
    return std::forward<SerializableType>(object);
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
    ElementType* xxdata;

public:
    using element_type = ElementType;

    // DONT use dereferencing of null data before rebinding
    alias_t() noexcept : xxdata(nullptr) {}

    template <typename OtherElementType,
              SF_REQUIRES(meta::is_static_castable<OtherElementType*, element_type*>::value)>
    alias_t(OtherElementType& data) noexcept : xxdata(std::addressof(data)) {}

    template <typename OtherElementType>
    alias_t(alias_t<OtherElementType> const& data) noexcept : alias_t(data.get()) {}

public:
    // rebinding data
    alias_t(alias_t const&) = default;
    alias_t& operator=(alias_t const&) = default;

    bool is_refer() const noexcept { return xxdata != nullptr; }

    template <typename OtherElementType>
    bool is_refer(OtherElementType& data)  const noexcept { return xxdata == std::addressof(data); }

    operator element_type&() const noexcept { return get(); }

    element_type& get() const noexcept { return *xxdata; }
    void set(element_type& data) noexcept { xxdata = std::addressof(data); }
};

} // namespace sf

TEMPLATE_SERIALIZATION(save, alias, template <typename ElementType>, ::sf::alias_t<ElementType>)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    if (not alias.is_refer())
        throw "The write alias_t must be initialized.";

    auto pointer = std::addressof(alias.get());
    auto const key = ::sf::detail::refer_key(archive, pointer);

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

    auto& address = archive.template tracking<::sf::tracking::raw_t>()[key];

    if (address == nullptr)
        throw "The read alias_t must be tracked before.";

    ElementType* pointer = nullptr;

    ::sf::detail::native_load(archive, pointer, address);

    alias.set(*pointer); // pointer will never nullptr
}

namespace sf
{

namespace utility
{

template <typename ValueType, std::size_t DimensionNumberValue>
class span_t;

template <typename ValueType, std::size_t DimensionNumberValue>
class span_base_t
{
public:
    using size_type = std::size_t;
    using value_type = span_t<ValueType, DimensionNumberValue - 1>;
    using pointer = typename meta::pointer<ValueType, DimensionNumberValue>::type;

protected:
    using dimension_size_type = size_type[DimensionNumberValue];

protected:
    alias_t<pointer> xxdata;
    dimension_size_type xxdimension_size;

protected:
    span_base_t(pointer& data) noexcept : xxdata(data), xxdimension_size() {}
    template <typename SizeType, typename... SizeTypes>
    span_base_t(pointer& data, SizeType dimension, SizeTypes... dimensions) noexcept
        : xxdata(data), xxdimension_size{dimension, dimensions...} {}

public:
    void init(pointer data) noexcept { xxdata.get() = data; }
    void data(alias_t<pointer> data) noexcept { xxdata = data; }

    pointer& data() noexcept { return xxdata; }
    size_type size() const noexcept { return xxdimension_size[0]; }
    dimension_size_type& dim() noexcept { return xxdimension_size; }
};

template <typename ValueType, std::size_t DimensionNumberValue>
class span_t : public span_base_t<ValueType, DimensionNumberValue>
{
protected:
    using BaseSpanType = span_base_t<ValueType, DimensionNumberValue>;

public:
    using typename BaseSpanType::size_type;
    using typename BaseSpanType::pointer;

public:
    using value_type = span_t<ValueType, DimensionNumberValue - 1>;
    using dereference_type = typename meta::pointer<ValueType, DimensionNumberValue - 1>::type;

protected:
    using typename BaseSpanType::dimension_size_type;

private:
    mutable value_type xxchild_scope;

public:
    template <typename SizeType, typename... SizeTypes,
              SF_REQUIRES(meta::negation<std::is_array<SizeType>>::value)>
    span_t(pointer& data, SizeType dimension, SizeTypes... dimensions) noexcept
        : BaseSpanType(data, dimension, dimensions...), xxchild_scope(data[0], dimensions...) {}

    void size(size_type value) noexcept { this->xxdimension_size[0] = value; }

    value_type& operator[] (size_type i) noexcept
    {
        xxchild_scope.data(this->xxdata[i]);
        return xxchild_scope;
    }

public:
    using BaseSpanType::size; // prevent Core function hiding
};

template <typename ValueType>
class span_t<ValueType, 1> : public span_base_t<ValueType, 1>
{
protected:
    using BaseSpanType = span_base_t<ValueType, 1>;

public:
    using typename BaseSpanType::size_type;
    using typename BaseSpanType::pointer;

public:
    using value_type = ValueType;
    using dereference_type = ValueType;

protected:
    using typename BaseSpanType::dimension_size_type;

public:
    span_t(pointer& data, size_type size) noexcept : BaseSpanType(data, size) {}

    dereference_type& operator[] (size_type i) noexcept { return this->xxdata[i]; }
};

} // namespace utility

namespace meta
{

template <typename> struct is_span : std::false_type {};
template <typename ValueType, std::size_t DimensionNumberValue>
struct is_span<utility::span_t<ValueType, DimensionNumberValue>> : std::true_type {};

template <typename PointerType, typename SizeType, typename... SizeTypes>
struct is_span_set
    : meta::all<std::integral_constant<bool, meta::pointer_count<PointerType>::value >= sizeof...(SizeTypes) + 1>,
                meta::all<std::is_arithmetic<SizeType>,
                          std::is_arithmetic<SizeTypes>...>> {};

} // namespace meta

namespace utility
{

template <typename PointerType, typename SizeType, typename... SizeTypes,
          std::size_t DimensionNumberValue = sizeof...(SizeTypes) + 1,
          typename Type = typename meta::remove_pointer<PointerType, DimensionNumberValue>::type,
          SF_REQUIRES(meta::is_span_set<PointerType, SizeType, SizeTypes...>::value)>
span_t<Type, DimensionNumberValue> make_span(PointerType& data, SizeType dimension, SizeTypes... dimensions)
{
    using size_type = typename span_t<Type, DimensionNumberValue>::size_type;
    return { data, static_cast<size_type>(dimension), static_cast<size_type>(dimensions)... };
}

} // namespace utility

namespace detail
{

template <class ArchiveType, typename SpanType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::negation<meta::is_span<SpanType>>>::value)>
void span_impl(ArchiveType& archive, SpanType& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class ArchiveType, typename SpanType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_span<SpanType>>::value)>
void span_impl(ArchiveType& archive, SpanType& array)
{
    using size_type = typename SpanType::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

template <class ArchiveType, typename SpanType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_span<SpanType>>::value)>
void span_impl(ArchiveType& archive, SpanType& array)
{
    using size_type = typename SpanType::size_type;
    using dereference_type = typename SpanType::dereference_type;

    auto pointer = new dereference_type [array.size()] {};
    array.init(pointer);

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class ArchiveType, typename PointerType,
          typename SizeType, typename... SizeTypes,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::is_span_set<PointerType, SizeType, SizeTypes...>>::value)>
void span(ArchiveType& archive, PointerType& pointer, SizeType& dimension, SizeTypes&... dimensions)
{
    if (not detail::refer_key(archive, pointer)) return; // serialize refer info
    archive(dimension, dimensions...);

    auto span_data = utility::make_span(pointer, dimension, dimensions...);
    detail::span_impl(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename PointerType, typename SizeType, typename... SizeTypes>
struct span_functor_t : apply_functor_t
{
    using PackType = std::tuple<PointerType&, SizeType&, SizeTypes&...>;

    PackType pack;

    span_functor_t(PointerType& pointer, SizeType& dimension, SizeTypes&... dimensions) noexcept
        : pack(pointer, dimension, dimensions...) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const
    {
        invoke(archive, meta::make_index_sequence<std::tuple_size<PackType>::value>{});
    }

private:
    template <class ArchiveType, std::size_t... SizeTypeIndexValues>
    void invoke(ArchiveType& archive, meta::index_sequence<SizeTypeIndexValues...>) const
    {
        span(archive, std::get<SizeTypeIndexValues>(pack)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename PointerType, typename SizeType, typename... SizeTypes,
          SF_REQUIRES(meta::is_span_set<PointerType, SizeType, SizeTypes...>::value)>
apply::span_functor_t<PointerType, SizeType, SizeTypes...> span(PointerType& pointer, SizeType& dimension, SizeTypes&... dimensions) noexcept
{
    return { pointer, dimension, dimensions... };
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

template <class ArchiveType, typename FieldType, typename enable = void>
struct bitpack_t;

template <class ArchiveType, typename FieldType>
struct bitpack_t<ArchiveType, FieldType, typename std::enable_if<sf::meta::is_oarchive<ArchiveType>::value>::type>
{
    ArchiveType& archive;
    FieldType data{};
    std::size_t offset{};

    bitpack_t(ArchiveType& archive) : archive(archive) {}
    ~bitpack_t() { archive & data; }

    FieldType operator()(FieldType field, std::size_t bits) noexcept
    {
        // same as data = data | (field << offset);
        data |= field << offset;
        offset += bits;

        return field;
    }
};

template <class ArchiveType, typename FieldType>
struct bitpack_t<ArchiveType, FieldType, typename std::enable_if<sf::meta::is_iarchive<ArchiveType>::value>::type>
{
    ArchiveType& archive;
    FieldType data{};

    bitpack_t(ArchiveType& archive) : archive(archive) { archive & data; }

    FieldType operator()(FieldType field, std::size_t bits) noexcept
    {
        //same as field = data & ~(0xf...f << bits)
        field = data & ~(~FieldType{} << bits);
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

TEMPLATE_SERIALIZATION(saveload, array,
    (template <typename ValueType, std::size_t SizeValue>), std::array<ValueType, SizeValue>)
{
    ::sf::compress::zip(archive, array);
}

#include <string> // basic_string

TEMPLATE_SERIALIZATION(save, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>)
{
    ::sf::let::u64 size = string.size();
    archive & size;

    ::sf::compress::zip(archive, string);
}

TEMPLATE_SERIALIZATION(load, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>)
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

template <class ArchiveType, typename... ArgumentTypes, std::size_t... TupleElementIndexes>
void expand(ArchiveType& archive, std::tuple<ArgumentTypes...>& tuple, meta::index_sequence<TupleElementIndexes...>)
{
    archive(std::get<TupleElementIndexes>(tuple)...);
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
    template <typename KeyType, typename ComparatorType, typename AllocatorType>                        \
    struct is_std_##set_type<std::set_type<KeyType, ComparatorType, AllocatorType>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_SET_TYPE_META_GENERIC(set)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_set)
SF_IS_STD_SET_TYPE_META_GENERIC(multiset)
SF_IS_STD_SET_TYPE_META_GENERIC(unordered_multiset)

template <class StdSetType> struct is_std_any_unordered_set
    : one<is_std_unordered_set<StdSetType>,
          is_std_unordered_multiset<StdSetType>> {};

template <class StdSetType> struct is_std_any_set
    : one<is_std_set<StdSetType>,
          is_std_multiset<StdSetType>,
          is_std_any_unordered_set<StdSetType>> {};

} // namespace meta

namespace detail
{

template <class StdSetType,
          SF_REQUIRES(meta::negation<meta::is_std_any_unordered_set<StdSetType>>::value)>
void reserve_unordered(StdSetType&, std::size_t) noexcept { /*pass*/ }

template <class StdSetType,
          SF_REQUIRES(meta::is_std_any_unordered_set<StdSetType>::value)>
void reserve_unordered(StdSetType& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, set, ::sf::meta::is_std_any_set<S>::value)
{
    ::sf::let::u64 size = set.size();
    archive & size;

    ::sf::compress::slow(archive, set);
}

CONDITIONAL_SERIALIZATION(load, set, ::sf::meta::is_std_any_set<S>::value)
{
    using value_type = typename S::value_type;

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
    (template <typename ElementType, typename DeleterType>), std::unique_ptr<ElementType, DeleterType>)
{
    auto data = unique_ptr.get();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, unique_ptr,
    (template <typename ElementType, typename DeleterType>), std::unique_ptr<ElementType, DeleterType>)
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
    template <typename KeyType, typename ValueType, typename ComparatorType, typename AllocatorType>    \
    struct is_std_##map_type<std::map_type<KeyType, ValueType, ComparatorType, AllocatorType>> : std::true_type {};

namespace sf
{

namespace meta
{

SF_IS_STD_MAP_TYPE_META_GENERIC(map)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_map)
SF_IS_STD_MAP_TYPE_META_GENERIC(multimap)
SF_IS_STD_MAP_TYPE_META_GENERIC(unordered_multimap)

template <class StdMapType> struct is_std_any_unordered_map
    : one<is_std_unordered_map<StdMapType>,
          is_std_unordered_multimap<StdMapType>> {};

template <class StdMapType> struct is_std_any_map
    : one<is_std_map<StdMapType>,
          is_std_multimap<StdMapType>,
          is_std_any_unordered_map<StdMapType>> {};

} // namespace meta

namespace detail
{

template <class StdMapType,
          SF_REQUIRES(meta::negation<meta::is_std_any_unordered_map<StdMapType>>::value)>
void reserve_unordered(StdMapType&, std::size_t) noexcept { /*pass*/ }

template <class StdMapType,
          SF_REQUIRES(meta::is_std_any_unordered_map<StdMapType>::value)>
void reserve_unordered(StdMapType& unordered, std::size_t size)
{
    unordered.reserve(size);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, map, ::sf::meta::is_std_any_map<S>::value)
{
    ::sf::let::u64 size = map.size();
    archive & size;

    ::sf::compress::slow(archive, map);
}

CONDITIONAL_SERIALIZATION(load, map, ::sf::meta::is_std_any_map<S>::value)
{
    using key_type   = typename S::key_type;
    using value_type = typename S::mapped_type;

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

template <template <typename...> class AdapterTemplate,
          typename ValueType, class ContainerType, typename... ArgumentTypes>
ContainerType& underlying(AdapterTemplate<ValueType, ContainerType, ArgumentTypes...>& adapter) noexcept
{
    using AdapterType = AdapterTemplate<ValueType, ContainerType, ArgumentTypes...>;

    struct xxinner : public AdapterType
    {
        static ContainerType& underlying(AdapterType& base)
        {
            return base.*(&xxinner::c);
        }
    };

    return xxinner::underlying(adapter);
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
TEMPLATE_SERIALIZATION(save, bitset, template <std::size_t SizeValue>, std::bitset<SizeValue>)
{
    auto data = bitset.to_string();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, bitset, template <std::size_t SizeValue>, std::bitset<SizeValue>)
{
    std::string data;
    archive & data;

    bitset = std::bitset<SizeValue>(data);
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

template <let::u64 ElementIndexValue, class ArchiveType, class VariantType,
          SF_REQUIRES(ElementIndexValue == std::variant_size<VariantType>::value)>
void variant_save(ArchiveType&, VariantType&, let::u64) noexcept { /*pass*/ }

template <let::u64 ElementIndexValue = 0, class ArchiveType, class VariantType,
          SF_REQUIRES(ElementIndexValue < std::variant_size<VariantType>::value)>
void variant_save(ArchiveType& archive, VariantType& variant, let::u64 index)
{
    if (ElementIndexValue < index) return variant_save<ElementIndexValue + 1>(archive, variant, index);
    archive & std::get<ElementIndexValue>(variant);
}

template <typename ElementType, class ArchiveType, class VariantType,
          SF_REQUIRES(meta::negation<std::is_constructible<ElementType>>::value)>
void variant_load_impl(ArchiveType&, VariantType&)
{
    throw "Require default constructor for specify type.";
}

template <typename ElementType, class ArchiveType, class VariantType,
          SF_REQUIRES(std::is_constructible<ElementType>::value)>
void variant_load_impl(ArchiveType& archive, VariantType& variant)
{
    archive & variant.template emplace<ElementType>();
}

template <let::u64 ElementIndexValue, class ArchiveType, class VariantType,
          SF_REQUIRES(ElementIndexValue == std::variant_size<VariantType>::value)>
void variant_load(ArchiveType&, VariantType&, let::u64) noexcept { /*pass*/ }

template <let::u64 ElementIndexValue = 0, class ArchiveType, class VariantType,
          SF_REQUIRES(ElementIndexValue < std::variant_size<VariantType>::value)>
void variant_load(ArchiveType& archive, VariantType& variant, let::u64 index)
{
    if (ElementIndexValue < index) return variant_load<ElementIndexValue + 1>(archive, variant, index);

    using type = typename std::variant_alternative<ElementIndexValue, VariantType>::type;
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
    ::sf::let::u64 hash = any.type().hash_code();
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
