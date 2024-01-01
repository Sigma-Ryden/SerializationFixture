#ifndef SF_BUILT_IN_ANY_HPP
#define SF_BUILT_IN_ANY_HPP

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

#include <type_traits> // true_type, false_type

#include <any> // any

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Hash.hpp>

#include <SF/Dynamic/AnyRegistry.hpp>

#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_any : std::false_type {};
template <> struct is_std_any<std::any> : std::true_type {};

} // namespace meta

inline namespace library
{

// please, use 'sf::serializable' for type any registry before std::any serialization
EXTERN_CONDITIONAL_SERIALIZATION(Save, any, meta::is_std_any<T>::value)
{
    let::u64 hash = SF_TYPE_HASH(any.type());
    archive & hash;

    dynamic::AnyRegistry::instance().save(archive, any, hash);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, any, meta::is_std_any<T>::value)
{
    let::u64 hash{};
    archive & hash;

    dynamic::AnyRegistry::instance().load(archive, any, hash);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any<T>::value)

#endif // if

#endif // SF_BUILT_IN_ANY_HPP
