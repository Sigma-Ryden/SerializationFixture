#ifndef SIRAF_SUPPORT_ANY_HPP
#define SIRAF_SUPPORT_ANY_HPP

#if __cplusplus >= 201703L

#include <type_traits> // true_type, false_type

#include <any> // any

#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/Core/TypeCore.hpp>
#include <Siraf/Core/Hash.hpp>

#include <Siraf/ExternSerialization.hpp>

#include <Siraf/Support/Detail/AnyRegistry.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_any : std::false_type {};
template <>
struct is_std_any<std::any> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, any, meta::is_std_any<T>::value)
{
    let::u64 hash = SIRAF_TYPE_HASH(any.type());
    archive & hash;

    AnyRegistry::instance().save(archive, any);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, any, meta::is_std_any<T>::value)
{
    let::u64 hash{};
    archive & hash;

    AnyRegistry::instance().load(archive, any, hash);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any<T>::value)

#endif // if

#endif // SIRAF_SUPPORT_ANY_HPP
