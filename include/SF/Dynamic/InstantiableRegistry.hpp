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
    ::sf::dynamic::instantiable_fixture_t<__VA_ARGS__> __fixture;                                       \
    public:

#define SERIALIZATION_TRAITS(...)                                                                       \
    private:                                                                                            \
    using __key_type = ::sf::core::instantiable_traits_t::key_type;                                     \
    static constexpr __key_type __static_traits() noexcept { return SF_STATIC_HASH(#__VA_ARGS__); }     \
    virtual __key_type __trait() const noexcept { return ::__sf::traits<__VA_ARGS__>(); }               \
    public:

namespace sf
{

namespace dynamic
{

class instantiable_registry_t
{
public:
    using key_type          = core::instantiable_traits_t::key_type;
    using instantiable_type = INSTANTIABLE_TYPE;
    using archive_type      = core::ioarchive_t;

private:
    template <typename ItemType>
    using InnerTable = std::unordered_map<key_type, ItemType>;

private:
    struct instantiable_proxy_t
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
    InnerTable<instantiable_proxy_t> registry_;

private:

    instantiable_registry_t() : registry_() {}

    ~instantiable_registry_t()
    {
        for (const auto& pair : registry_) delete pair.second.__instance;
    }

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

        proxy.__instance = memory::allocate_raw<instantiable_type, T>();

        proxy.__shared = [] { return memory::allocate_shared<instantiable_type, T>(); };

        proxy.__cast_shared = [](std::shared_ptr<void> address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.__raw = [] { return memory::allocate_raw<instantiable_type, T>(); };

        proxy.__cast_raw = [](void* address)
        {
            return memory::static_pointer_cast<instantiable_type, T>(address);
        };

        proxy.__save = [](archive_type& archive, instantiable_type* instance)
        {
            archive << *memory::dynamic_pointer_cast<T>(instance);
        };

        proxy.__load = [](archive_type& archive, instantiable_type* instance)
        {
            archive >> *memory::dynamic_pointer_cast<T>(instance);
        };

        registry_.emplace(key, proxy);
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_shared<TraitsType>::value)>
    std::shared_ptr<instantiable_type> clone(key_type key)
    {
        return registry(key).__shared();
    }

    template <typename TraitsType,
              SF_REQUIRE(meta::is_memory_raw<TraitsType>::value)>
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
              SF_REQUIRE(::__sf::has_save_mode<T>::value)>
    void save(archive_type& archive, Pointer& pointer)
    {
        auto key = ::__sf::traits(*pointer);
        registry(key).__save(archive, pointer);
    }

    template <typename Pointer, typename T = sf::meta::dereference<Pointer>,
              SF_REQUIRE(::__sf::has_load_mode<T>::value)>
    void load(archive_type& archive, Pointer& pointer)
    {
        auto key = ::__sf::traits(*pointer);
        registry(key).__load(archive, pointer);
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
    static bool lock_;

public:
    instantiable_fixture_t() { call<T>(); }

public:
    template <typename dT = T,
              SF_REQUIRE(instantiable_registry_t::is_instantiable<dT>::value)>
    static void call()
    {
        if (lock_) return;
        lock_ = true; // lock before creating clone instance to prevent recursive call

        auto& registry = instantiable_registry_t::instance();

        auto key = ::__sf::template traits<T>();
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
bool instantiable_fixture_t<T>::lock_ = false;

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_REGISTRY_HPP
