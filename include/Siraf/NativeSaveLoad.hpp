#ifndef SIRAF_NATIVE_SAVE_LOAD_HPP
#define SIRAF_NATIVE_SAVE_LOAD_HPP

#include <Siraf/Core/Access.hpp>

#include <Siraf/Memory/Memory.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace detail
{

template <class Archive, typename T, typename key_type,
          SIREQUIRE(meta::is_write_archive<Archive>() and
                    not meta::is_pointer_to_polymorphic<T>())>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
}

template <class Archive, typename T, typename key_type,
          SIREQUIRE(meta::is_write_archive<Archive>() and
                    meta::is_pointer_to_polymorphic<T>())>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_read_archive<Archive>() and
                    not meta::is_pointer_to_polymorphic<T>())>
void native_load(Archive& archive, T& pointer, memory::void_ptr<T>& address)
{
    using dT = meta::dereference<T>;
    memory::assign<dT>(pointer, address);
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_read_archive<Archive>() and
                    meta::is_pointer_to_polymorphic<T>())>
void native_load(Archive& archive, T& pointer, memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace siraf

#endif // SIRAF_NATIVE_SAVE_LOAD_HPP
