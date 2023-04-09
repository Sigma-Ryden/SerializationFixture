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

class AnyRegistry
{
public:
    using archive_type = core::ArchiveBase;

private:
    struct AnyProxy
    {
        // we use raw function ptr instead std::function to reach perfomance
        void(*__save)(archive_type&, std::any&) = nullptr;
        void(*__load)(archive_type&, std::any&) = nullptr;
    };

private:
    using InnerTable = std::unordered_map<let::u64, AnyProxy>;

private:
    InnerTable registry_;

private:
    AnyRegistry() : registry_() {}

    AnyRegistry(const AnyRegistry&) = delete;
    AnyRegistry& operator=(const AnyRegistry&) = delete;

public:
    static AnyRegistry& instance() noexcept
    {
        static AnyRegistry self;
        return self;
    }

    template <typename T> void update(let::u64 hash)
    {
        if (is_registered(hash)) return;

        AnyProxy proxy;

        proxy.__save = [](archive_type& archive, std::any& any)
        {
            archive << std::any_cast<T&>(any);
        };

        proxy.__load = [](archive_type& archive, std::any& any)
        {
            any.emplace<T>();
            archive >> std::any_cast<T&>(any);
        };

        registry_.emplace(hash, proxy);
    }

public:
    void save(archive_type& archive, std::any& any, let::u64 hash)
    {
        registry(hash).__save(archive, any);
    }

    void load(archive_type& archive, std::any& any, let::u64 hash)
    {
        registry(hash).__load(archive, any);
    }

public:
    bool is_registered(let::u64 hash)
    {
        return registry_.find(hash) != registry_.end();
    }

#ifdef SF_REGISTRY_ACCESS
private:
#endif // #ifdef SF_REGISTRY_ACCESS
    const AnyProxy& registry(let::u64 hash)
    {
        // It happens if the type not registered with fixture object.
        auto it = registry_.find(hash);
        if (it == registry_.end())
            throw "The 'sf::AnyRegistry' must registry type with specify hash code.";

        return it->second;
    }
};

template <class T>
class AnyFixture
{
private:
    static bool lock_;

public:
    AnyFixture() { call(); }

public:
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = AnyRegistry::instance();

        auto hash = SF_TYPE_HASH(typeid(T));
    #ifdef SF_DEBUG
        if (registry.is_registered(hash))
            throw "The 'sf::dynamic::AnyRegistry' must contains unique hashes.";
    #endif // SF_DEBUG

        registry.update<T>(hash);
    }
};

template <class T>
bool AnyFixture<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#endif // if

#endif // SF_DYNAMIC_ANY_REGISTRY_HPP
