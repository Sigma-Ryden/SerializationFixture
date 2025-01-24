#ifndef SF_DYNAMIC_ANY_REGISTRY_HPP
#define SF_DYNAMIC_ANY_REGISTRY_HPP

#if !defined(SF_ANY_SUPPORT_DISABLE)

#include <cstdint> // uint64_t

#include <unordered_map> // unordered_map
#include <any> // any

#include <SerializationFixture/Core/PolymorphicArchive.hpp>

#include <SerializationFixture/Detail/Meta.hpp>

namespace sf
{

namespace dynamic
{

struct any_registry_t
{
public:
    struct any_proxy_t
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*save)(ioarchive_t&, std::any&) = nullptr;
        void(*load)(ioarchive_t&, std::any&) = nullptr;
    };

public:
    std::unordered_map<std::uint64_t, any_proxy_t> all;

public:
    template <typename SerializableType>
    void add()
    {
        static auto lock = false; if (lock) return;
        lock = true;

        any_proxy_t proxy;

        proxy.save = [](ioarchive_t& archive, std::any& any)
        {
            archive << std::any_cast<SerializableType&>(any);
        };

        proxy.load = [](ioarchive_t& archive, std::any& any)
        {
            any.emplace<SerializableType>();
            archive >> std::any_cast<SerializableType&>(any);
        };

        all.emplace(typeid(SerializableType).hash_code(), proxy);
    }

    void save(ioarchive_t& archive, std::any& any, std::uint64_t hash);
    void load(ioarchive_t& archive, std::any& any, std::uint64_t hash);
};

extern any_registry_t any_registry;

} // namespace dynamic

} // namespace sf

#endif // if

#endif // SF_DYNAMIC_ANY_REGISTRY_HPP
