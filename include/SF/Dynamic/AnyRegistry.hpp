#ifndef SF_DYNAMIC_ANY_REGISTRY_HPP
#define SF_DYNAMIC_ANY_REGISTRY_HPP

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)

#include <unordered_map> // unordered_map
#include <any> // any
#include <typeinfo> // typeid, type_info

#include <SF/Core/PolymorphicArchive.hpp>
#include <SF/Core/TypeCore.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Detail/Meta.hpp>

namespace sf
{

namespace dynamic
{

class any_registry_t
{
public:
    using archive_type = core::ioarchive_t;

private:
    struct any_proxy_t
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*save)(archive_type&, std::any&) = nullptr;
        void(*load)(archive_type&, std::any&) = nullptr;
    };

private:
    using InnerTable = std::unordered_map<let::u64, any_proxy_t>;

private:
    InnerTable registry_;

private:
    any_registry_t() : registry_() {}

    any_registry_t(const any_registry_t&) = delete;
    any_registry_t& operator=(const any_registry_t&) = delete;

public:
    static any_registry_t& instance() noexcept
    {
        static any_registry_t self;
        return self;
    }

    template <typename T> void update(let::u64 hash)
    {
        if (is_registered(hash)) return;

        any_proxy_t proxy;

        proxy.save = [](archive_type& archive, std::any& any)
        {
            archive << std::any_cast<T&>(any);
        };

        proxy.load = [](archive_type& archive, std::any& any)
        {
            any.emplace<T>();
            archive >> std::any_cast<T&>(any);
        };

        registry_.emplace(hash, proxy);
    }

public:
    void save(archive_type& archive, std::any& any, let::u64 hash)
    {
        registry(hash).save(archive, any);
    }

    void load(archive_type& archive, std::any& any, let::u64 hash)
    {
        registry(hash).load(archive, any);
    }

public:
    bool is_registered(let::u64 hash)
    {
        return registry_.find(hash) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    const any_proxy_t& registry(let::u64 hash)
    {
        // It happens if the type not registered with fixture object.
        auto it = registry_.find(hash);
        if (it == registry_.end())
            throw "The 'sf::any_registry_t' must registry type with specify hash code.";

        return it->second;
    }
};

template <typename T>
class any_fixture_t
{
private:
    static bool lock;

public:
    any_fixture_t() { call(); }

public:
    static void call()
    {
        if (lock) return;
        lock = true; // lock before creating clone instance to prevent recursive call

        auto& registry = any_registry_t::instance();

        auto hash = SF_TYPE_HASH(typeid(T));
    #ifdef SF_DEBUG
        if (registry.is_registered(hash))
            throw "The 'sf::dynamic::any_registry_t' must contains unique hashes.";
    #endif // SF_DEBUG

        registry.update<T>(hash);
    }
};

template <typename T>
bool any_fixture_t<T>::lock = false;

} // namespace dynamic

} // namespace sf

#endif // if

#endif // SF_DYNAMIC_ANY_REGISTRY_HPP
