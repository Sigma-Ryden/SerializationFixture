#ifndef SF_BUILT_IN_ANY_HPP
#define SF_BUILT_IN_ANY_HPP

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

#include <any> // any

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Hash.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Dynamic/AnyRegistry.hpp>

// please, use 'sf::serializable' for type any registry before std::any serialization
SERIALIZATION(save, any, std::any)
{
    ::sf::let::u64 hash = SF_TYPE_HASH(any.type());
    archive & hash;

    ::sf::dynamic::any_registry_t::instance().save(archive, any, hash);
}

SERIALIZATION(load, any, std::any)
{
    ::sf::let::u64 hash{};
    archive & hash;

    ::sf::dynamic::any_registry_t::instance().load(archive, any, hash);
}

#endif // if

#endif // SF_BUILT_IN_ANY_HPP
