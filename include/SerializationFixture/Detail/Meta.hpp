#ifndef SF_DETAIL_META_HPP
#define SF_DETAIL_META_HPP

#include <cstddef> // size_t

#include <type_traits>
// is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#if __cplusplus >= 201703L
#include <any> // any
#endif // if

#include <memory> // shared_ptr
#include <array> // array

#include <SerializationFixture/Detail/MetaMacro.hpp>

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

#endif // SF_DETAIL_META_HPP
