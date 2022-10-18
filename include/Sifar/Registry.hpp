#ifndef SIFAR_REGISTRY_HPP
#define SIFAR_REGISTRY_HPP

#include <Sifar/Access.hpp>
#include <Sifar/FactoryTable.hpp>

#include <Sifar/Utility.hpp>

#include <Sifar/RegistryBase.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace dynamic
{

class ExternRegistry : public RegistryBase
{
public:
    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(Access::is_registered_class<T>())>
    static void save(core::ArchiveBase& archive, Pointer& pointer, key_type key)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        Access::dynamic_save(archive, pointer);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(Access::is_registered_class<T>())>
    static void load(core::ArchiveBase& archive, Pointer& pointer, key_type key)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto cloned = FactoryTable::instance().clone(key);
        pointer = dynamic_cast<Pointer>(cloned);

        Access::dynamic_load(archive, pointer);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(Access::is_registered_class<T>())>
    static void assign(Pointer& pointer, void* address, key_type key)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        auto casted = FactoryTable::instance().cast(address, key);
        pointer = dynamic_cast<Pointer>(casted);
    }
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_REGISTRY_HPP
