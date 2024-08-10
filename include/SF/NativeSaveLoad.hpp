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

template <class ArchiveType, typename T, typename KeyType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_save(ArchiveType& archive, T& pointer, KeyType track_key) noexcept { /*pass*/ }

template <class ArchiveType, typename T, typename KeyType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<T>>::value)>
void native_save(ArchiveType& archive, T& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class ArchiveType, typename T,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_load(ArchiveType& archive, T& pointer, memory::void_ptr<T>& address) noexcept
{
    memory::assign<typename meta::dereference<T>::type>(pointer, address);
}

template <class ArchiveType, typename T,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<T>>::value)>
void native_load(ArchiveType& archive, T& pointer, memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

#endif // SF_NATIVE_SAVE_LOAD_HPP
