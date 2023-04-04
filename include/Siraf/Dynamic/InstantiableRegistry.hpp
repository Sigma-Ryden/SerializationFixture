#ifndef SIRAF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
#define SIRAF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP

#include <unordered_map> // unordered_map

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/Memory.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/PolymorphicArchive.hpp>

#include <Siraf/Dynamic/Instantiable.hpp>

namespace siraf
{

namespace dynamic
{

class InstantiableRegistry
{
public:
    using key_type          = core::PolymorphicTraitBase::key_type;
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
    template <typename dT> static constexpr bool is_instantiable() noexcept
    {
        // we should use external check
        return meta::is_cast_allowed<dT*, instantiable_type*>();
    }

public:
    template <class T, SIREQUIRE(not is_instantiable<T>())>
    void update(key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'Instantiable'.";
    }

    template <class T, SIREQUIRE(is_instantiable<T>())>
    void update(key_type key)
    {
        using I = instantiable_type;

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
              SIREQUIRE(meta::is_memory_shared<TraitType>())>
    std::shared_ptr<instantiable_type> clone(key_type key)
    {
        return registry(key).__shared();
    }

    template <typename TraitType,
              SIREQUIRE(meta::is_memory_raw<TraitType>())>
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

    template <typename Pointer, typename T = siraf::meta::dereference<Pointer>,
              SIREQUIRE(::Serialization::has_save_mode<T>())>
    void save(archive_type& archive, Pointer& pointer)
    {
        auto key = ::Serialization::trait(*pointer);
        registry(key).__save(archive, pointer);
    }

    template <typename Pointer, typename T = siraf::meta::dereference<Pointer>,
              SIREQUIRE(::Serialization::has_load_mode<T>())>
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

private:
    InstantiableProxy& registry(key_type key)
    {
        // It happens if the class with the given key has not beed public inherited
        // from the instantiable class or not registered with fixture object.
        auto it = registry_.find(key);
        if (it == registry_.end())
            throw "The 'siraf::dynamic::InstantiableRegistry' does not has instance with input key.";

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

private:
    template <typename dT = T,
              SIREQUIRE(InstantiableRegistry::is_instantiable<dT>())>
    void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = InstantiableRegistry::instance();

        auto key = ::Serialization::template trait<T>();
    #ifdef SIRAF_DEBUG
        if (registry.is_registered(key))
            throw "The 'siraf::dynamic::InstantiableRegistry' must contains instance with unique key.";
    #endif // SIRAF_DEBUG

        registry.update<T>(key);
    }

    template <typename dT = T,
              SIREQUIRE(not InstantiableRegistry::is_instantiable<dT>())>
    void call() { /*pass*/ }
};

template <class T>
bool InstantiableFixture<T>::lock_ = false;

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP