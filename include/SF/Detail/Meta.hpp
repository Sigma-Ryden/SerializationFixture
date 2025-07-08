#ifndef SF_DETAIL_META_HPP
#define SF_DETAIL_META_HPP

#include <cstddef> // size_t

#include <type_traits>
// is_arithmetic, is_array, is_pointer,
// enable_if, is_same, true_type, false_type

#include <utility> // index_sequence, make_index_sequence

#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace meta
{

template <typename...> constexpr bool to_false() noexcept { return false; }

template <typename Type, std::size_t IndexableValue = 0>
auto declval() noexcept -> decltype(std::declval<Type>()) { return std::declval<Type>(); }

template <typename Type, typename... OtherTypes> struct is_same : std::conjunction<std::is_same<Type, OtherTypes>...> {};

template <typename Type, std::size_t PointeringNumberValue = 1> struct remove_pointer { using type = Type; };
template <typename Type> struct remove_pointer<Type*, 1> { using type = Type; };

template <typename Type, std::size_t PointeringNumberValue>
struct remove_pointer<Type*, PointeringNumberValue>
{
    using type = typename remove_pointer<Type, PointeringNumberValue - 1>::type;
};

template <typename Type, std::size_t PointeringNumberValue = 1>
struct pointer { using type = typename pointer<Type, PointeringNumberValue - 1>::type*; };
template <typename Type> struct pointer<Type, 0> { using type = Type; };

// meta
template <typename, typename enable = void> struct is_complete : std::false_type {};
template <typename Type> struct is_complete<Type, std::void_t<decltype(sizeof(Type))>> : std::true_type {};

template <typename FromType, typename ToType, typename enable = void>
struct is_static_castable : std::false_type {};

template <typename FromType, typename ToType>
struct is_static_castable<FromType, ToType, std::void_t<decltype( static_cast<ToType>(std::declval<FromType>()) )>> : std::true_type {};

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
    template <typename Type> operator Type() const; // not impl
};

template <class AggregateType, typename SequenceType = std::index_sequence<>, typename overload = void>
struct aggregate_size : SequenceType {};

template <class AggregateType, std::size_t... FieldIndexValues>
struct aggregate_size<AggregateType, std::index_sequence<FieldIndexValues...>,
                      std::void_t<decltype(AggregateType{ declval<dummy_t>(), declval<dummy_t, FieldIndexValues>()... })>>
    : aggregate_size<AggregateType, std::index_sequence<FieldIndexValues..., sizeof...(FieldIndexValues)>> {};

template <class, typename enable = void>
struct object_value { using type = dummy_t; };

template <class SerializableObjectType>
struct object_value<SerializableObjectType, std::void_t<typename SerializableObjectType::value_type>>
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
    : std::conjunction<std::is_base_of<BaseType, DerivedType>,
                       std::is_base_of<BaseTypes, DerivedType>...> {};

template <class BaseType, class DerivedType> struct is_virtual_base_of
    : std::conjunction<std::is_base_of<BaseType, DerivedType>,
                       std::negation<is_static_castable<BaseType*, DerivedType*>>> {};

template <typename> struct is_void_pointer : std::false_type {};
template <> struct is_void_pointer<void*> : std::true_type {};

template <typename> struct is_null_pointer : std::false_type {};
template <> struct is_null_pointer<std::nullptr_t> : std::true_type {};

template <typename> struct is_function_pointer : std::false_type {};
template <typename ReturnType, typename... ArgumentTypes>
struct is_function_pointer<ReturnType (*)(ArgumentTypes...)> : std::true_type {};

template <typename SerializableType> struct is_unsupported :
    std::disjunction
    <
        is_void_pointer<SerializableType>,
        is_function_pointer<SerializableType>,
        is_null_pointer<SerializableType>,
        std::is_function<SerializableType>,
        std::is_member_function_pointer<SerializableType>,
        std::is_member_object_pointer<SerializableType>,
        std::is_reference<SerializableType>
    > {};

} // namespace meta

} // namespace sf

#endif // SF_DETAIL_META_HPP
