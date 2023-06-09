#ifndef SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
#define SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP

#include <unordered_map> // unordered_map

#include <SF/Core/Serialization.hpp>
#include <SF/Core/Memory.hpp>
#include <SF/Core/InstantiableTrait.hpp>

#include <SF/Core/PolymorphicArchive.hpp>

#include <SF/Dynamic/Instantiable.hpp>

namespace sf
{

namespace dynamic
{

class InstantiableRegistry
{
public:
    using key_type          = core::InstantiableTraitBase::key_type;
    using instantiable_type = INSTANTIABLE_TYPE;
    using archive_type      = core::ArchiveBase;

private:
    template <typename ItemType>
    using InnerTable = std::unordered_map<key_type, ItemType>;

private:
    struct InstantiableProxy
    {
        instantiable_type* __instance = nullptr;

        std::shared_ptr<instantiable_type>(*__shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*__cast_shared)(std::shared_ptr<void>) = nullptr;

        instantiable_type*(*__raw)() = nullptr;
        instantiable_type*(*__cast_raw)(void*) = nullptr;

        void(*__save)(archive_type&, instantiable_type*) = nullptr;
        void(*__load)(archive_type&, instantiable_type*) = nullptr;
    };

private:
    InnerTable<InstantiableProxy> registry_;

private:

    InstantiableRegistry() : registry_() {}

    ~InstantiableRegistry()
    {
        for (const auto& pair : registry_) delete pair.second.__instance;
    }

    InstantiableRegistry(const InstantiableRegistry&) = delete;
    InstantiableRegistry& operator=(const InstantiableRegistry&) = delete;

public:
    static InstantiableRegistry& instance() noexcept
    {
        static InstantiableRegistry self;
        return self;
    }

public:
    // we should use external check
    template <typename dT> struct is_instantiable : meta::is_cast_allowed<dT*, instantiable_type*> {};

public:
    template <class T, SFREQUIRE(not is_instantiable<T>::value)>
    void update(key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'Instantiable'.";
    }

    template <class T, SFREQUIRE(is_instantiable<T>::value)>
    void update(key_type key)
    {
        if (is_registered(key)) return;

        InstantiableProxy proxy;

        proxy.__instance = Memory::allocate_raw<instantiable_type, T>();

        proxy.__shared = [] { return Memory::allocate_shared<instantiable_type, T>(); };

        proxy.__cast_shared = [](std::shared_ptr<void> address)
        {
            return Memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.__raw = [] { return Memory::allocate_raw<instantiable_type, T>(); };

        proxy.__cast_raw = [](void* address)
        {
            return Memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.__save = [](archive_type& archive, instantiable_type* instance)
        {
            archive << *Memory::dynamic_pointer_cast<T>(instance);
        };

        proxy.__load = [](archive_type& archive, instantiable_type* instance)
        {
            archive >> *Memory::dynamic_pointer_cast<T>(instance);
        };

        registry_.emplace(key, proxy);
    }

    template <typename TraitType,
              SFREQUIRE(meta::is_memory_shared<TraitType>::value)>
    std::shared_ptr<instantiable_type> clone(key_type key)
    {
        return registry(key).__shared();
    }

    template <typename TraitType,
              SFREQUIRE(meta::is_memory_raw<TraitType>::value)>
    instantiable_type* clone(key_type key)
    {
        return registry(key).__raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> address, key_type key)
    {
        return registry(key).__cast_shared(address);
    }

    instantiable_type* cast(void* address, key_type key)
    {
        return registry(key).__cast_raw(address);
    }

    template <typename Pointer, typename T = sf::meta::dereference<Pointer>,
              SFREQUIRE(::Serialization::has_save_mode<T>::value)>
    void save(archive_type& archive, Pointer& pointer)
    {
        auto key = ::Serialization::trait(*pointer);
        registry(key).__save(archive, pointer);
    }

    template <typename Pointer, typename T = sf::meta::dereference<Pointer>,
              SFREQUIRE(::Serialization::has_load_mode<T>::value)>
    void load(archive_type& archive, Pointer& pointer)
    {
        auto key = ::Serialization::trait(*pointer);
        registry(key).__load(archive, pointer);
    }

public:
    bool is_registered(key_type key)
    {
        return registry_.find(key) != registry_.end();
    }

#ifdef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    InstantiableProxy& registry(key_type key)
    {
        // It happens if the class with the given key has not beed public inherited
        // from the instantiable class or not registered with fixture object.
        auto it = registry_.find(key);
        if (it == registry_.end())
            throw "The 'sf::dynamic::InstantiableRegistry' does not has instance with input key.";

        return it->second;
    }
};

template <class T>
class InstantiableFixture
{
private:
    static bool lock_;

public:
    InstantiableFixture() { call<T>(); }

public:
    template <typename dT = T,
              SFREQUIRE(InstantiableRegistry::is_instantiable<dT>::value)>
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = InstantiableRegistry::instance();

        auto key = ::Serialization::template trait<T>();
    #ifdef SF_DEBUG
        if (registry.is_registered(key))
            throw "The 'sf::dynamic::InstantiableRegistry' must contains instance with unique key.";
    #endif // SF_DEBUG

        registry.update<T>(key);
    }

    template <typename dT = T,
              SFREQUIRE(not InstantiableRegistry::is_instantiable<dT>::value)>
    static void call() noexcept { /*pass*/ }
};

template <class T>
bool InstantiableFixture<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
