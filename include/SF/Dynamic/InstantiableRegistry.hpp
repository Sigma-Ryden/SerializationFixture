#ifndef SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
#define SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP

#include <unordered_map> // unordered_map

#include <SF/Core/Serialization.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Core/Hash.hpp>

#include <SF/Core/PolymorphicArchive.hpp>

#include <SF/Dynamic/Instantiable.hpp>
#include <SF/Dynamic/InstantiableTraits.hpp>

namespace sf
{

namespace dynamic
{

class instantiable_registry_t
{
public:
    using instantiable_type = INSTANTIABLE_TYPE;

public:
    struct instantiable_proxy_t
    {
        std::shared_ptr<instantiable_type>(*shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*cast_shared)(std::shared_ptr<void>) = nullptr;

        instantiable_type*(*raw)() = nullptr;
        instantiable_type*(*cast_raw)(void*) = nullptr;

        void(*save)(core::ioarchive_t&, instantiable_type*) = nullptr;
        void(*load)(core::ioarchive_t&, instantiable_type*) = nullptr;
    };

public:
    std::unordered_map<::xxsf_instantiable_traits_key_type, instantiable_proxy_t*> all;
    std::unordered_map<std::size_t, instantiable_proxy_t*> rtti_all;
    std::unordered_map<std::size_t, ::xxsf_instantiable_traits_key_type> rtti;

private:
    instantiable_registry_t() = default;
    instantiable_registry_t(const instantiable_registry_t&) = delete;
    instantiable_registry_t& operator=(const instantiable_registry_t&) = delete;

public:
    static instantiable_registry_t& instance() noexcept
    {
        static instantiable_registry_t self;
        return self;
    }

    ~instantiable_registry_t()
    {
        for (auto key_and_proxy : all) delete key_and_proxy.second;
    }

public:
    // we should use external check
    template <typename dT> struct is_instantiable : meta::is_cast_allowed<dT*, instantiable_type*> {};

public:
    template <class T, SF_REQUIRE(not is_instantiable<T>::value)>
    void update(::xxsf_instantiable_traits_key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'instantiable_t'.";
    }

    template <class T, SF_REQUIRE(is_instantiable<T>::value)>
    void update(::xxsf_instantiable_traits_key_type key)
    {
        if (all.find(key) != all.end()) return;

        auto proxy = new instantiable_proxy_t;

        proxy->shared = [] { return memory::allocate_shared<instantiable_type, T>(); };

        proxy->cast_shared = [](std::shared_ptr<void> address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy->raw = [] { return memory::allocate_raw<instantiable_type, T>(); };

        proxy->cast_raw = [](void* address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy->save = [](core::ioarchive_t& archive, instantiable_type* instance)
        {
            archive << *memory::dynamic_pointer_cast<T>(instance);
        };

        proxy->load = [](core::ioarchive_t& archive, instantiable_type* instance)
        {
            archive >> *memory::dynamic_pointer_cast<T>(instance);
        };

        all.emplace(key, proxy);

        const auto hash = typeid(T).hash_code();
        rtti_all.emplace(hash, proxy);
        rtti.emplace(hash, key);
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_shared<TraitsType>::value)>
    std::shared_ptr<instantiable_type> clone(::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key)->shared();
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_raw<TraitsType>::value)>
    instantiable_type* clone(::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key)->raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> address, ::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key)->cast_shared(address);
    }

    instantiable_type* cast(void* address, ::xxsf_instantiable_traits_key_type key) const
    {
        return all.at(key)->cast_raw(address);
    }

    template <typename Pointer>
    void save(core::ioarchive_t& archive, Pointer& pointer) const
    {
        const auto key = SF_TYPE_HASH(*pointer);
        rtti_all.at(key)->save(archive, pointer);
    }

    template <typename Pointer>
    void load(core::ioarchive_t& archive, Pointer& pointer) const
    {
        const auto key = SF_TYPE_HASH(*pointer);
        rtti_all.at(key)->load(archive, pointer);
    }
};

template <class T>
class instantiable_fixture_t
{
private:
    static bool lock_;

public:
    instantiable_fixture_t() { call<T>(); }

public:
    template <typename dT = T,
              SF_REQUIRE(instantiable_registry_t::is_instantiable<dT>::value)>
    static bool call()
    {
        if (lock_) return true;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = instantiable_registry_t::instance();

        const auto key = ::xxsf_instantiable_traits<T>::key;
    #ifdef SF_DEBUG
        if (key == ::xxsf_instantiable_traits_base_key)
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with valid key.";

        if (registry.all.find(key) != registry.all.end())
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with unique key.";
    #endif // SF_DEBUG

        registry.update<T>(key);
        return true;
    }

    template <typename dT = T,
              SF_REQUIRE(not instantiable_registry_t::is_instantiable<dT>::value)>
    static bool call() noexcept { return false; }
};

template <class T>
bool instantiable_fixture_t<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
