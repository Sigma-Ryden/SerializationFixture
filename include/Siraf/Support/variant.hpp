#ifndef SIRAF_SUPPORT_VARIANT_HPP
#define SIRAF_SUPPORT_VARIANT_HPP

#if __cplusplus >= 201703L

#include <type_traits> // true_type, false_type

#include <variant> // variant

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Utility.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_variant : std::false_type {};
template <typename... Tn>
struct is_std_variant<std::variant<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <typename Type, class Variant,
          SIREQUIRE(not std::is_constructible<Type>::value)>
void variant_emplace(Variant& variant)
{
    throw "Require default constructor for specify type.";
}

template <typename Type, class Variant,
          SIREQUIRE(std::is_constructible<Type>::value)>
void variant_emplace(Variant& variant)
{
    variant.template emplace<Type>();
}

template <let::u64 I, class Variant,
          SIREQUIRE(I == std::variant_size<Variant>::value)>
void variant_construct(Variant& variant, let::u64 index) noexcept
{ /*pass*/ }

template <let::u64 I = 0, class Variant,
          SIREQUIRE(I < std::variant_size<Variant>::value)>
void variant_construct(Variant& variant, let::u64 index)
{
    if (I < index) return variant_construct<I + 1>(variant, index);

    using type = typename std::variant_alternative<I, Variant>::type;
    variant_emplace<type>(variant);
}

template <class Archive>
class SerializationVisitor
{
private:
    Archive& archive_;

public:
    SerializationVisitor(Archive& archive) : archive_(archive) {}

    void operator()(std::monostate& data) noexcept { /*pass*/ }

    template <typename T>
    void operator()(T& data) { archive_ & data; }
};

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, variant, meta::is_std_variant<T>::value)
{
    let::u64 index = variant.index();
    archive & index;

    if (index != std::variant_npos)
        std::visit(detail::SerializationVisitor(archive), variant);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, variant, meta::is_std_variant<T>::value)
{
    let::u64 index{};
    archive & index;

    if (index != std::variant_npos)
    {
        detail::variant_construct(variant, index);
        std::visit(detail::SerializationVisitor(archive), variant);
    }

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_variant<T>::value)

#endif // if

#endif // SIRAF_SUPPORT_VARIANT_HPP
