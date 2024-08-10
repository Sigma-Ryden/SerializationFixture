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

#endif // SF_BUILT_IN_VARIANT_HPP
