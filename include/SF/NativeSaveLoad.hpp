#ifndef SF_NATIVE_SAVE_LOAD_HPP
#define SF_NATIVE_SAVE_LOAD_HPP

#include <SF/Core/Memory.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace detail
{

template <class Archive, typename T, typename key_type,
          SFREQUIRE(meta::is_write_archive<Archive>() and
                    not meta::is_pointer_to_polymorphic<T>())>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
}

template <class Archive, typename T, typename key_type,
          SFREQUIRE(meta::is_write_archive<Archive>() and
                    meta::is_pointer_to_polymorphic<T>())>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename T,
          SFREQUIRE(meta::is_read_archive<Archive>() and
                    not meta::is_pointer_to_polymorphic<T>())>
void native_load(Archive& archive, T& pointer, Memory::void_ptr<T>& address)
{
    using dT = meta::dereference<T>;
    Memory::assign<dT>(pointer, address);
}

template <class Archive, typename T,
          SFREQUIRE(meta::is_read_archive<Archive>() and
                    meta::is_pointer_to_polymorphic<T>())>
void native_load(Archive& archive, T& pointer, Memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

#endif // SF_NATIVE_SAVE_LOAD_HPP
