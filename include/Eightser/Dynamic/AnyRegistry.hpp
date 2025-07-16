#ifndef EIGHTSER_DYNAMIC_ANY_REGISTRY_HPP
#define EIGHTSER_DYNAMIC_ANY_REGISTRY_HPP

#if !defined(EIGHTSER_ANY_SUPPORT_DISABLE)

#include <cstdint> // uint64_t

#include <unordered_map> // unordered_map
#include <any> // any

#include <Eightser/Core/PolymorphicArchive.hpp>

#include <Eightser/Detail/Meta.hpp>

namespace eightser
{

namespace dynamic
{

struct any_registry_t final
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

} // namespace eightser

#endif // if

#endif // EIGHTSER_DYNAMIC_ANY_REGISTRY_HPP
