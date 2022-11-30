#ifndef SIFAR_NATIVE_SAVE_LOAD_HPP
#define SIFAR_NATIVE_SAVE_LOAD_HPP

#include <Sifar/Access.hpp>

#include <Sifar/Memory.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace detail
{

template <class Archive, typename Pointer, typename key_type,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_write_archive<Archive>()
                    and not Access::is_registered_class<T>())>
void native_save(Archive& archive, Pointer& pointer, key_type track_key)
{
    archive & track_key;
}

template <class Archive, typename Pointer, typename key_type,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_write_archive<Archive>()
                    and Access::is_registered_class<T>())>
void native_save(Archive& archive, Pointer& pointer, key_type track_key)
{
    archive & track_key;
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename Pointer, typename VoidPointer,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_read_archive<Archive>()
                    and not Access::is_registered_class<T>())>
void native_load(Archive& archive, Pointer& pointer, VoidPointer& address)
{
    memory::assign<T>(pointer, address);
}

template <class Archive, typename Pointer, typename VoidPointer,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_read_archive<Archive>()
                    and Access::is_registered_class<T>())>
void native_load(Archive& archive, Pointer& pointer, VoidPointer& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sifar

#endif // SIFAR_NATIVE_SAVE_LOAD_HPP
