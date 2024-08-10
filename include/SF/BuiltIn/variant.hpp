#ifndef SF_BUILT_IN_VARIANT_HPP
#define SF_BUILT_IN_VARIANT_HPP

#if __cplusplus >= 201703L

#include <variant> // variant

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

// serialization of std::monostate
#include <SF/Aggregate.hpp>

namespace sf
{

namespace detail
{

template <let::u64 I, class ArchiveType, class Variant,
          SF_REQUIRES(I == std::variant_size<Variant>::value)>
void variant_save(ArchiveType& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class ArchiveType, class Variant,
          SF_REQUIRES(I < std::variant_size<Variant>::value)>
void variant_save(ArchiveType& archive, Variant& variant, let::u64 index)
{
    if (I < index) return variant_save<I + 1>(archive, variant, index);
    archive & std::get<I>(variant);
}

template <typename Type, class ArchiveType, class Variant,
          SF_REQUIRES(not std::is_constructible<Type>::value)>
void variant_load_impl(ArchiveType& archive, Variant& variant)
{
    throw "Require default constructor for specify type.";
}

template <typename Type, class ArchiveType, class Variant,
          SF_REQUIRES(std::is_constructible<Type>::value)>
void variant_load_impl(ArchiveType& archive, Variant& variant)
{
    archive & variant.template emplace<Type>();
}

template <let::u64 I, class ArchiveType, class Variant,
          SF_REQUIRES(I == std::variant_size<Variant>::value)>
void variant_load(ArchiveType& archive, Variant& variant, let::u64 index) noexcept { /*pass*/ }

template <let::u64 I = 0, class ArchiveType, class Variant,
          SF_REQUIRES(I < std::variant_size<Variant>::value)>
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

#endif // SF_BUILT_IN_VARIANT_HPP
