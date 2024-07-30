#ifndef SF_BUILT_IN_ANY_HPP
#define SF_BUILT_IN_ANY_HPP

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

#include <type_traits> // true_type, false_type

#include <any> // any

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Hash.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Dynamic/AnyRegistry.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_any : std::false_type {};
template <> struct is_std_any<std::any> : std::true_type {};

} // namespace meta

} // namespace sf

// please, use 'sf::serializable' for type any registry before std::any serialization
CONDITIONAL_SERIALIZATION(save, any, ::sf::meta::is_std_any<T>::value)
{
    ::sf::let::u64 hash = SF_TYPE_HASH(any.type());
    archive & hash;

    ::sf::dynamic::any_registry_t::instance().save(archive, any, hash);
}

CONDITIONAL_SERIALIZATION(load, any, ::sf::meta::is_std_any<T>::value)
{
    ::sf::let::u64 hash{};
    archive & hash;

    ::sf::dynamic::any_registry_t::instance().load(archive, any, hash);
}

#endif // if

#endif // SF_BUILT_IN_ANY_HPP
