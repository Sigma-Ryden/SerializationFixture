#ifndef EIGHTSER_BUILT_IN_VARIANT_HPP
#define EIGHTSER_BUILT_IN_VARIANT_HPP

#include <cstdint> // uint64_t

#include <variant> // variant

#include <Eightser/Core/Serialization.hpp>

// serialization of std::monostate
#include <Eightser/Aggregate.hpp>

namespace eightser
{

namespace detail
{

template <std::uint64_t ElementIndexValue, class ArchiveType, class VariantType,
          EIGHTSER_REQUIRES(ElementIndexValue == std::variant_size<VariantType>::value)>
void variant_save(ArchiveType&, VariantType&, std::uint64_t) noexcept { /*pass*/ }

template <std::uint64_t ElementIndexValue = 0, class ArchiveType, class VariantType,
          EIGHTSER_REQUIRES(ElementIndexValue < std::variant_size<VariantType>::value)>
void variant_save(ArchiveType& archive, VariantType& variant, std::uint64_t index)
{
    if (ElementIndexValue < index) return variant_save<ElementIndexValue + 1>(archive, variant, index);
    archive & std::get<ElementIndexValue>(variant);
}

template <typename ElementType, class ArchiveType, class VariantType,
          EIGHTSER_REQUIRES(std::negation<std::is_constructible<ElementType>>::value)>
void variant_load_impl(ArchiveType&, VariantType&)
{
    throw "Require default constructor for specify type.";
}

template <typename ElementType, class ArchiveType, class VariantType,
          EIGHTSER_REQUIRES(std::is_constructible<ElementType>::value)>
void variant_load_impl(ArchiveType& archive, VariantType& variant)
{
    archive & variant.template emplace<ElementType>();
}

template <std::uint64_t ElementIndexValue, class ArchiveType, class VariantType,
          EIGHTSER_REQUIRES(ElementIndexValue == std::variant_size<VariantType>::value)>
void variant_load(ArchiveType&, VariantType&, std::uint64_t) noexcept { /*pass*/ }

template <std::uint64_t ElementIndexValue = 0, class ArchiveType, class VariantType,
          EIGHTSER_REQUIRES(ElementIndexValue < std::variant_size<VariantType>::value)>
void variant_load(ArchiveType& archive, VariantType& variant, std::uint64_t index)
{
    if (ElementIndexValue < index) return variant_load<ElementIndexValue + 1>(archive, variant, index);

    using type = typename std::variant_alternative<ElementIndexValue, VariantType>::type;
    variant_load_impl<type>(archive, variant);
}

} // namespace detail

} // namespace eightser

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename... ArgumentTypes>, std::variant<ArgumentTypes...>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, variant, template <typename... ArgumentTypes>, std::variant<ArgumentTypes...>)
    std::uint64_t index = variant.index();
    archive & index;

    if (index != std::variant_npos) ::eightser::detail::variant_save(archive, variant, index);
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, variant, (template <typename... ArgumentTypes>), std::variant<ArgumentTypes...>)
    std::uint64_t index{};
    archive & index;

    if (index != std::variant_npos) ::eightser::detail::variant_load(archive, variant, index);
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_VARIANT_HPP
