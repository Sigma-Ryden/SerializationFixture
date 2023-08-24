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

template <class OArchive, typename T, typename KeyType,
          SFREQUIRE(meta::all<meta::is_oarchive<OArchive>,
                              meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_save(OArchive& archive, T& pointer, KeyType track_key) noexcept { /*pass*/ }

template <class OArchive, typename T, typename KeyType,
          SFREQUIRE(meta::all<meta::is_oarchive<OArchive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void native_save(OArchive& archive, T& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class IArchive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<IArchive>,
                              meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_load(IArchive& archive, T& pointer, Memory::void_ptr<T>& address) noexcept
{
    Memory::assign<meta::dereference<T>>(pointer, address);
}

template <class IArchive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<IArchive>,
                              meta::is_pointer_to_polymorphic<T>>::value)>
void native_load(IArchive& archive, T& pointer, Memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

#endif // SF_NATIVE_SAVE_LOAD_HPP
