#ifndef SIFAR_NATIVE_SAVE_LOAD_HPP
#define SIFAR_NATIVE_SAVE_LOAD_HPP

#include <Sifar/Access.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace detail
{

template <class Archive, typename T, typename key_type,
          typename U = meta::dereference<T>,
          SIREQUIRE(meta::is_write_archive<Archive>()
                    and not Access::is_registered_class<U>())>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
}

template <class Archive, typename T, typename key_type,
          typename U = meta::dereference<T>,
          SIREQUIRE(meta::is_write_archive<Archive>()
                    and Access::is_registered_class<U>())>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename T,
          typename U = meta::dereference<T>,
          SIREQUIRE(meta::is_read_archive<Archive>()
                    and not Access::is_registered_class<U>())>
void native_load(Archive& archive, T& pointer, void* address)
{
    pointer = static_cast<T>(address);
}

template <class Archive, typename T,
          typename U = meta::dereference<T>,
          SIREQUIRE(meta::is_read_archive<Archive>()
                    and Access::is_registered_class<U>())>
void native_load(Archive& archive, T& pointer, void* address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sifar

#endif // SIFAR_NATIVE_SAVE_LOAD_HPP
