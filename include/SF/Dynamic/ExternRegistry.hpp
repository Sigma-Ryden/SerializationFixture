#ifndef SF_DYNAMIC_REGISTRY_HPP
#define SF_DYNAMIC_REGISTRY_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/InstantiableTraits.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/AnyRegistry.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace dynamic
{

class extern_registry_t
{
public:
    using key_type = core::instantiable_traits_t::key_type;

public:
    template <class Archive, typename T,
              SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static key_type save_key(Archive& archive, T& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto key = ::__sf::traits(*pointer);
        archive & key;

        return key;
    }

    template <class Archive, typename T,
              SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static key_type load_key(Archive& archive, T& pointer)
    {
    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        key_type key{};
        archive & key;

        return key;
    }

private:
    template <typename T> struct is_pointer_to_instantiable
        : meta::all<instantiable_registry_t::is_instantiable<meta::dereference<T>>,
                    meta::is_pointer_to_polymorphic<T>> {};

public:
    template <typename T, SF_REQUIRE(is_pointer_to_instantiable<T>::value)>
    static void save(core::ioarchive_t& archive, T& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = memory::raw(pointer);
        instantiable_registry_t::instance().save(archive, raw_pointer);
    }

    template <typename T, SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void load(core::ioarchive_t& archive, T& pointer, key_type key, memory::void_ptr<T>& cache)
    {
        using TraitsType = typename memory::ptr_traits<T>::traits;
        using dT = meta::dereference<T>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto& registry = instantiable_registry_t::instance();

        auto cloned = registry.clone<TraitsType>(key);

        pointer = memory::dynamic_pointer_cast<dT>(cloned);
        cache = memory::pure(pointer);

        auto raw_pointer = memory::raw(pointer);
        registry.load(archive, raw_pointer);
    }

    template <typename T, SF_REQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void assign(T& pointer, const memory::void_ptr<T>& address, key_type key)
    {
        using dT = meta::dereference<T>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto casted = instantiable_registry_t::instance().cast(address, key);
        pointer = memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_REGISTRY_HPP

