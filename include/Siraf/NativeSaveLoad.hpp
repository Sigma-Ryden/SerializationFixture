#ifndef SIRAF_NATIVE_SAVE_LOAD_HPP
#define SIRAF_NATIVE_SAVE_LOAD_HPP

#include <Siraf/Access.hpp>

#include <Siraf/Memory/Memory.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace detail
{

template <class Archive, typename Pointer, typename key_type,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_write_archive<Archive>() and
                    not meta::is_pointer_to_polymorphic<Pointer>())>
void native_save(Archive& archive, Pointer& pointer, key_type track_key)
{
    archive & track_key;
}

template <class Archive, typename Pointer, typename key_type,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_write_archive<Archive>() and
                    meta::is_pointer_to_polymorphic<Pointer>())>
void native_save(Archive& archive, Pointer& pointer, key_type track_key)
{
    archive & track_key;
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename Pointer, typename VoidPointer,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_read_archive<Archive>() and
                    not meta::is_pointer_to_polymorphic<Pointer>())>
void native_load(Archive& archive, Pointer& pointer, VoidPointer& address)
{
    memory::assign<T>(pointer, address);
}

template <class Archive, typename Pointer, typename VoidPointer,
          typename T = meta::dereference<Pointer>,
          SIREQUIRE(meta::is_read_archive<Archive>() and
                    meta::is_pointer_to_polymorphic<Pointer>())>
void native_load(Archive& archive, Pointer& pointer, VoidPointer& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace siraf

#endif // SIRAF_NATIVE_SAVE_LOAD_HPP
D_HPP
