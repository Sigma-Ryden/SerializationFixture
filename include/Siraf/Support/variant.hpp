#ifndef SIRAF_SUPPORT_VARIANT_HPP
#define SIRAF_SUPPORT_VARIANT_HPP

#if __cplusplus >= 201703L

#include <type_traits> // true_type, false_type

#include <variant> // variant

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_variant : std::false_type {};
template <typename... Tn>
struct is_std_variant<std::variant<Tn...>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, variant, meta::is_std_variant<T>::value)
{
    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, variant, meta::is_std_variant<T>::value)
{
    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_variant<T>::value)

#endif // if

#endif // SIRAF_SUPPORT_VARIANT_HPP
