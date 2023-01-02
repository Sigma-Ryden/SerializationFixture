#ifndef SIRAF_DYNAMIC_REGISTRY_HPP
#define SIRAF_DYNAMIC_REGISTRY_HPP

#include <Siraf/Access.hpp>
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
    using key_type = PolymorphicTraitCore::key_type;

public:
    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(meta::is_pointer<Pointer>() and Access::is_registered_class<T>())>
    static void save(core::ArchiveBase& archive, Pointer& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        Access::dynamic_save(archive, pointer);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(meta::is_pointer<Pointer>() and Access::is_registered_class<T>())>
    static void load(core::ArchiveBase& archive, Pointer& pointer, key_type key)
    {
        using TraitType = typename memory::ptr_trait<Pointer>::trait;

        if (pointer != nullptr)
            throw "The read shared pointer must be initialized to nullptr.";

        auto cloned = FactoryTable::instance().clone<TraitType>(key);

    #ifdef SIRAF_DEBUG
        if (cloned == nullptr)
            throw "The 'siraf::dynamic::FactoryTable' does not have clone instance with input key.";
    #endif // SIRAF_DEBUG

        pointer = memory::dynamic_pointer_cast<T>(cloned);

        auto raw_pointer = memory::raw(pointer);
        Access::dynamic_load(archive, raw_pointer);
    }

    template <typename Pointer,
              typename VoidPointer = typename memory::ptr_trait<Pointer>::void_ptr,
              typename T = meta::dereference<Pointer>,
              SIREQUIRE(meta::is_pointer<Pointer>() and Access::is_registered_class<T>())>
    static void assign(Pointer& pointer, const VoidPointer& address, key_type key)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto casted = FactoryTable::instance().cast(address, key);
        pointer = memory::dynamic_pointer_cast<T>(casted);
    }
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_REGISTRY_HPP
