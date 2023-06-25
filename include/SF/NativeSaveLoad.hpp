#ifndef SF_NATIVE_SAVE_LOAD_HPP
#define SF_NATIVE_SAVE_LOAD_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace detail
{

template <class WriteArchive, typename T, typename KeyType,
          SFREQUIRE(meta::all<meta::is_write_archive<WriteArchive>,
                              meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_save(WriteArchive& archive, T& pointer, KeyType track_key) noexcept { /*pass*/ }

template <class WriteArchive, typename T, typename KeyType,
          SFREQUIRE(meta::all<meta::is_write_archive<WriteArchive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void native_save(WriteArchive& archive, T& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class ReadArchive, typename T,
          SFREQUIRE(meta::all<meta::is_read_archive<ReadArchive>,
                              meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_load(ReadArchive& archive, T& pointer, Memory::void_ptr<T>& address) noexcept
{
    Memory::assign<meta::dereference<T>>(pointer, address);
}

template <class ReadArchive, typename T,
          SFREQUIRE(meta::all<meta::is_read_archive<ReadArchive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void native_load(ReadArchive& archive, T& pointer, Memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

#endif // SF_NATIVE_SAVE_LOAD_HPP
