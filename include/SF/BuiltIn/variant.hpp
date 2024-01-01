#ifndef SF_BUILT_IN_VARIANT_HPP
#define SF_BUILT_IN_VARIANT_HPP

#if __cplusplus >= 201703L

#include <type_traits> // true_type, false_type

#include <variant> // variant

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/ExternSerialization.hpp>

// serialization of std::monostate
#include <SF/Aggregate.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_variant : std::false_type {};
template <typename... Tn> struct is_std_variant<std::variant<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <let::u64 I, class Archive, class Variant,
          SFREQUIRE(I == std::variant_size<Variant>::value)>
void variant_save(Archive& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class Archive, class Variant,
          SFREQUIRE(I < std::variant_size<Variant>::value)>
void variant_save(Archive& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_save<I + 1>(archive, variant, index);
    archive & std::get<I>(variant);
}

template <typename Type, class Archive, class Variant,
          SFREQUIRE(not std::is_constructible<Type>::value)>
void variant_load_implementation(Archive& archive, Variant& variant)
{
    throw "Require default constructor for specify type.";
}

template <typename Type, class Archive, class Variant,
          SFREQUIRE(std::is_constructible<Type>::value)>
void variant_load_implementation(Archive& archive, Variant& variant)
{
    archive & variant.template emplace<Type>();
}

template <let::u64 I, class Archive, class Variant,
          SFREQUIRE(I == std::variant_size<Variant>::value)>
void variant_load(Archive& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class Archive, class Variant,
          SFREQUIRE(I < std::variant_size<Variant>::value)>
void variant_load(Archive& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_load<I + 1>(archive, variant, index);

    using type = typename std::variant_alternative<I, Variant>::type;
    variant_load_implementation<type>(archive, variant);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, variant, meta::is_std_variant<T>::value)
{
    let::u64 index = variant.index();
    archive & index;

    if (index != std::variant_npos)
        detail::variant_save(archive, variant, index);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, variant, meta::is_std_variant<T>::value)
{
    let::u64 index{};
    archive & index;

    if (index != std::variant_npos)
        detail::variant_load(archive, variant, index);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_variant<T>::value)

#endif // if

#endif // SF_BUILT_IN_VARIANT_HPP
