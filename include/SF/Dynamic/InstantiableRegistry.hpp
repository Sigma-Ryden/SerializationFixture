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

private:
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

private:
    instantiable_registry_t() : registry_() {}
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
        for (auto proxy : all) delete proxy;
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
        if (is_registered(key)) return;

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
        rtti_all.emplace(typeid(T), proxy);
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_shared<TraitsType>::value)>
    std::shared_ptr<instantiable_type> clone(::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).shared();
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_raw<TraitsType>::value)>
    instantiable_type* clone(::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> address, ::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).cast_shared(address);
    }

    instantiable_type* cast(void* address, ::xxsf_instantiable_traits_key_type key)
    {
        return registry(key).cast_raw(address);
    }

    template <typename Pointer>
    void save(core::ioarchive_t& archive, Pointer& pointer)
    {
        const auto key = SF_TYPE_HASH(*pointer);
        registry(key).save(archive, pointer);
    }

    template <typename Pointer>
    void load(core::ioarchive_t& archive, Pointer& pointer)
    {
        const auto key = SF_TYPE_HASH(*pointer);
        registry(key).load(archive, pointer);
    }

public:
    bool is_registered(::xxsf_instantiable_traits_key_type key)
    {
        return registry_.find(key) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    instantiable_proxy_t* registry(::xxsf_instantiable_traits_key_type key)
    {
        // It happens if the class with the given key has not beed public inherited
        // from the instantiable class or not registered with fixture object.
        auto it = registry_.find(key);
        if (it == registry_.end())
            throw "The 'sf::dynamic::instantiable_registry_t' does not has instance with input key.";

        return it->second;
    }
};

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
