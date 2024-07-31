#ifndef SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
#define SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP

#include <unordered_map> // unordered_map

#include <SF/Core/Serialization.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Core/InstantiableTraits.hpp>
#include <SF/Core/Hash.hpp>

#include <SF/Core/PolymorphicArchive.hpp>

#include <SF/Dynamic/Instantiable.hpp>

// Auto instantiable type registration
#define SERIALIZATION_FIXTURE(...)                                                                      \
    private:                                                                                            \
    ::sf::dynamic::instantiable_fixture_t<__VA_ARGS__> xxfixture;                                       \
    public:

#define SERIALIZATION_TRAITS(...)                                                                       \
    private:                                                                                            \
    using xxkey_type = ::xxsf_instantiable_traits<void>::key_type;                                      \
    static constexpr xxkey_type xxstatic_traits() noexcept { return SF_STATIC_HASH(#__VA_ARGS__); }     \
    virtual xxkey_type xxtrait() const noexcept { return ::xxsf::traits<__VA_ARGS__>(); }               \
    public:

namespace sf
{

namespace dynamic
{

class instantiable_registry_t
{
public:
    using key_type          = ::xxsf_instantiable_traits<void>::key_type;
    using instantiable_type = INSTANTIABLE_TYPE;
    using archive_type      = core::ioarchive_t;

private:
    template <typename ItemType>
    using InnerTable = std::unordered_map<key_type, ItemType>;

private:
    struct instantiable_proxy_t
    {
        std::shared_ptr<instantiable_type>(*shared)() = nullptr;
        std::shared_ptr<instantiable_type>(*cast_shared)(std::shared_ptr<void>) = nullptr;

        instantiable_type*(*raw)() = nullptr;
        instantiable_type*(*cast_raw)(void*) = nullptr;

        void(*save)(archive_type&, instantiable_type*) = nullptr;
        void(*load)(archive_type&, instantiable_type*) = nullptr;
    };

private:
    InnerTable<instantiable_proxy_t> registry_;

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

public:
    // we should use external check
    template <typename dT> struct is_instantiable : meta::is_cast_allowed<dT*, instantiable_type*> {};

public:
    template <class T, SF_REQUIRE(not is_instantiable<T>::value)>
    void update(key_type key)
    {
        throw "The polymorphic 'T' type is not convertible to 'instantiable_t'.";
    }

    template <class T, SF_REQUIRE(is_instantiable<T>::value)>
    void update(key_type key)
    {
        if (is_registered(key)) return;

        instantiable_proxy_t proxy;

        proxy.shared = [] { return memory::allocate_shared<instantiable_type, T>(); };

        proxy.cast_shared = [](std::shared_ptr<void> address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.raw = [] { return memory::allocate_raw<instantiable_type, T>(); };

        proxy.cast_raw = [](void* address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.save = [](archive_type& archive, instantiable_type* instance)
        {
            archive << *memory::dynamic_pointer_cast<T>(instance);
        };

        proxy.load = [](archive_type& archive, instantiable_type* instance)
        {
            archive >> *memory::dynamic_pointer_cast<T>(instance);
        };

        registry_.emplace(key, proxy);
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_shared<TraitsType>::value)>
    std::shared_ptr<instantiable_type> clone(key_type key)
    {
        return registry(key).shared();
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_raw<TraitsType>::value)>
    instantiable_type* clone(key_type key)
    {
        return registry(key).raw();
    }

    std::shared_ptr<instantiable_type> cast(std::shared_ptr<void> address, key_type key)
    {
        return registry(key).cast_shared(address);
    }

    instantiable_type* cast(void* address, key_type key)
    {
        return registry(key).cast_raw(address);
    }

    template <typename Pointer/*,
              SF_REQUIRE(meta::is_has_any_save_mode<typename meta::dereference<Pointer>::type>::value)*/>
    void save(archive_type& archive, Pointer& pointer)
    {
        const auto key = ::xxsf::traits(*pointer);
        registry(key).save(archive, pointer);
    }

    template <typename Pointer/*,
              SF_REQUIRE(meta::is_has_any_load_mode<typename meta::dereference<Pointer>::type>::value)*/>
    void load(archive_type& archive, Pointer& pointer)
    {
        const auto key = ::xxsf::traits(*pointer);
        registry(key).load(archive, pointer);
    }

public:
    bool is_registered(key_type key)
    {
        return registry_.find(key) != registry_.end();
    }

#ifndef SF_REGISTRY_ACCESS
private:
#endif // SF_REGISTRY_ACCESS
    instantiable_proxy_t& registry(key_type key)
    {
        // It happens if the class with the given key has not beed public inherited
        // from the instantiable class or not registered with fixture object.
        auto it = registry_.find(key);
        if (it == registry_.end())
            throw "The 'sf::dynamic::instantiable_registry_t' does not has instance with input key.";

        return it->second;
    }
};

template <class T>
class instantiable_fixture_t
{
private:
    static bool lock;

public:
    instantiable_fixture_t() { call<T>(); }

public:
    template <typename dT = T,
              SF_REQUIRE(instantiable_registry_t::is_instantiable<dT>::value)>
    static void call()
    {
        if (lock) return;
        lock = true; // lock before creating clone instance to prevent recursive call

        auto& registry = instantiable_registry_t::instance();

        const auto key = ::xxsf::template traits<T>();
    #ifdef SF_DEBUG
        if (registry.is_registered(key))
            throw "The 'sf::dynamic::instantiable_registry_t' must contains instance with unique key.";
    #endif // SF_DEBUG

        registry.update<T>(key);
    }

    template <typename dT = T,
              SF_REQUIRE(not instantiable_registry_t::is_instantiable<dT>::value)>
    static void call() noexcept { /*pass*/ }
};

template <class T>
bool instantiable_fixture_t<T>::lock = false;

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
