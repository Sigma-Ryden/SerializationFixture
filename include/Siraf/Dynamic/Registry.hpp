#ifndef SIRAF_DYNAMIC_REGISTRY_HPP
#define SIRAF_DYNAMIC_REGISTRY_HPP

#include <Siraf/Core/Access.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>

#include <Siraf/Memory/Memory.hpp>

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
              SIREQUIRE(meta::is_pointer<T>() and core::Access::is_registered_class<dT>())>
    static void save(core::ArchiveBase& archive, T& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        core::Access::dynamic_save(archive, pointer);
    }

    template <typename T, typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>() and core::Access::is_registered_class<dT>())>
    static void load(core::ArchiveBase& archive, T& pointer, key_type key, memory::void_ptr<T>& cache)
    {
        using TraitType = typename memory::ptr_trait<T>::trait;

        if (pointer != nullptr)
            throw "The read shared pointer must be initialized to nullptr.";

        auto cloned = FactoryTable::instance().clone<TraitType>(key);

    #ifdef SIRAF_DEBUG
        if (cloned == nullptr)
            throw "The 'siraf::dynamic::FactoryTable' does not has clone instance with input key.";
    #endif // SIRAF_DEBUG

        pointer = memory::dynamic_pointer_cast<dT>(cloned);
        cache = memory::pure(pointer);

        auto raw_pointer = memory::raw(pointer);
        core::Access::dynamic_load(archive, raw_pointer);
    }

    template <typename T,
              typename dT = meta::dereference<T>,
              SIREQUIRE(meta::is_pointer<T>() and core::Access::is_registered_class<dT>())>
    static void assign(T& pointer, const memory::void_ptr<T>& address, key_type key)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto casted = FactoryTable::instance().cast(address, key);
        pointer = memory::dynamic_pointer_cast<dT>(casted);
    }
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_REGISTRY_HPP

