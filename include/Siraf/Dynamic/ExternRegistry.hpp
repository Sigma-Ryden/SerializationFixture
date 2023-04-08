#ifndef SIRAF_DYNAMIC_REGISTRY_HPP
#define SIRAF_DYNAMIC_REGISTRY_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/Memory.hpp>

#include <Siraf/Dynamic/InstantiableRegistry.hpp>
#include <Siraf/Dynamic/AnyRegistry.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace dynamic
{

class ExternRegistry
{
public:
    using key_type = core::PolymorphicTraitBase::key_type;

public:
    template <class Archive, class Pointer,
              SIREQUIRE(meta::is_pointer_to_polymorphic<Pointer>())>
    static key_type save_key(Archive& archive, Pointer& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto key = ::Serialization::trait(*pointer);
        archive & key;

        return key;
    }

    template <class Archive, class Pointer,
              SIREQUIRE(meta::is_pointer_to_polymorphic<Pointer>())>
    static key_type load_key(Archive& archive, Pointer& pointer)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        key_type key;
        archive & key;

        return key;
    }

public:
    template <typename T, typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>())>
    static void save(core::ArchiveBase& archive, T& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto raw_pointer = Memory::raw(pointer);
        InstantiableRegistry::instance().save(archive, raw_pointer);
    }

    template <typename T, typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>())>
    static void load(core::ArchiveBase& archive, T& pointer, key_type key, Memory::void_ptr<T>& cache)
    {
        using TraitType = typename Memory::ptr_trait<T>::trait;

        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto& registry = InstantiableRegistry::instance();

        auto cloned = registry.clone<TraitType>(key);

        pointer = Memory::dynamic_pointer_cast<dT>(cloned);
        cache = Memory::pure(pointer);

        auto raw_pointer = Memory::raw(pointer);
        registry.load(archive, raw_pointer);
    }

    template <typename T,
              typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>())>
    static void assign(T& pointer, const Memory::void_ptr<T>& address, key_type key)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto casted = InstantiableRegistry::instance().cast(address, key);
        pointer = Memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_REGISTRY_HPP

