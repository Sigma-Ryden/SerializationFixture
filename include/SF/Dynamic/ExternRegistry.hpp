#ifndef SF_DYNAMIC_REGISTRY_HPP
#define SF_DYNAMIC_REGISTRY_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/InstantiableTrait.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/AnyRegistry.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace dynamic
{

class ExternRegistry
{
public:
    using key_type = core::InstantiableTraitBase::key_type;

public:
    template <class Archive, class Pointer,
              SFREQUIRE(meta::is_pointer_to_polymorphic<Pointer>::value)>
    static key_type save_key(Archive& archive, Pointer& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto key = ::Serialization::trait(*pointer);
        archive & key;

        return key;
    }

    template <class Archive, class Pointer,
              SFREQUIRE(meta::is_pointer_to_polymorphic<Pointer>::value)>
    static key_type load_key(Archive& archive, Pointer& pointer)
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
        : meta::all<InstantiableRegistry::is_instantiable<meta::dereference<T>>,
                    meta::is_pointer_to_polymorphic<T>> {};

public:
    template <typename T, SFREQUIRE(is_pointer_to_instantiable<T>::value)>
    static void save(core::ArchiveBase& archive, T& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = Memory::raw(pointer);
        InstantiableRegistry::instance().save(archive, raw_pointer);
    }

    template <typename T, SFREQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void load(core::ArchiveBase& archive, T& pointer, key_type key, Memory::void_ptr<T>& cache)
    {
        using TraitType = typename Memory::ptr_trait<T>::trait;
        using dT = meta::dereference<T>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto& registry = InstantiableRegistry::instance();

        auto cloned = registry.clone<TraitType>(key);

        pointer = Memory::dynamic_pointer_cast<dT>(cloned);
        cache = Memory::pure(pointer);

        auto raw_pointer = Memory::raw(pointer);
        registry.load(archive, raw_pointer);
    }

    template <typename T, SFREQUIRE(meta::is_pointer_to_polymorphic<T>::value)>
    static void assign(T& pointer, const Memory::void_ptr<T>& address, key_type key)
    {
        using dT = meta::dereference<T>;

    #ifndef SF_GARBAGE_CHECK_DISABLE
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";
    #endif // SF_GARBAGE_CHECK_DISABLE

        auto casted = InstantiableRegistry::instance().cast(address, key);
        pointer = Memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace sf

#endif // SF_DYNAMIC_REGISTRY_HPP

