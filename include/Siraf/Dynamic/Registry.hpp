#ifndef SIRAF_DYNAMIC_REGISTRY_HPP
#define SIRAF_DYNAMIC_REGISTRY_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/Memory.hpp>

#include <Siraf/Dynamic/RegistryBase.hpp>
#include <Siraf/Dynamic/FactoryTable.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace dynamic
{

class ExternRegistry : public RegistryBase
{
public:
    using key_type = core::PolymorphicTraitBase::key_type;

public:
    template <typename T, typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>() and ::Serialization::is_registered_class<dT>())>
    static void save(core::ArchiveBase& archive, T& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        ::Serialization::dynamic_save(archive, pointer);
    }

    template <typename T, typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>() and ::Serialization::is_registered_class<dT>())>
    static void load(core::ArchiveBase& archive, T& pointer, key_type key, Memory::void_ptr<T>& cache)
    {
        using TraitType = typename Memory::ptr_trait<T>::trait;

        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto cloned = FactoryTable::instance().clone<TraitType>(key);

        pointer = Memory::dynamic_pointer_cast<dT>(cloned);
        cache = Memory::pure(pointer);

        auto raw_pointer = Memory::raw(pointer);
        ::Serialization::dynamic_load(archive, raw_pointer);
    }

    template <typename T,
              typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>() and ::Serialization::is_registered_class<dT>())>
    static void assign(T& pointer, const Memory::void_ptr<T>& address, key_type key)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto casted = FactoryTable::instance().cast(address, key);
        pointer = Memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_REGISTRY_HPP

