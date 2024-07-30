#ifndef SF_BUILT_IN_VARIANT_HPP
#define SF_BUILT_IN_VARIANT_HPP

#if __cplusplus >= 201703L

#include <type_traits> // true_type, false_type

#include <variant> // variant

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

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
          SF_REQUIRE(I == std::variant_size<Variant>::value)>
void variant_save(Archive& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class Archive, class Variant,
          SF_REQUIRE(I < std::variant_size<Variant>::value)>
void variant_save(Archive& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_save<I + 1>(archive, variant, index);
    archive & std::get<I>(variant);
}

template <typename Type, class Archive, class Variant,
          SF_REQUIRE(not std::is_constructible<Type>::value)>
void variant_load_impl(Archive& archive, Variant& variant)
{
    throw "Require default constructor for specify type.";
}

template <typename Type, class Archive, class Variant,
          SF_REQUIRE(std::is_constructible<Type>::value)>
void variant_load_impl(Archive& archive, Variant& variant)
{
    archive & variant.template emplace<Type>();
}

template <let::u64 I, class Archive, class Variant,
          SF_REQUIRE(I == std::variant_size<Variant>::value)>
void variant_load(Archive& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class Archive, class Variant,
          SF_REQUIRE(I < std::variant_size<Variant>::value)>
void variant_load(Archive& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_load<I + 1>(archive, variant, index);

    using type = typename std::variant_alternative<I, Variant>::type;
    variant_load_impl<type>(archive, variant);
}

} // namespace detail

} // namespace sf

CONDITIONAL_SERIALIZATION(save, variant, ::sf::meta::is_std_variant<T>::value)
{
    ::sf::let::u64 index = variant.index();
    archive & index;

    if (index != std::variant_npos)
        ::sf::detail::variant_save(archive, variant, index);
}

CONDITIONAL_SERIALIZATION(load, variant, ::sf::meta::is_std_variant<T>::value)
{
    ::sf::let::u64 index{};
    archive & index;

    if (index != std::variant_npos)
        ::sf::detail::variant_load(archive, variant, index);
}

#endif // if

#endif // SF_BUILT_IN_VARIANT_HPP
