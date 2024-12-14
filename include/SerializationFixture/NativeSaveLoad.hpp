#ifndef SF_NATIVE_SAVE_LOAD_HPP
#define SF_NATIVE_SAVE_LOAD_HPP

#include <SerializationFixture/Core/ArchiveBase.hpp>
#include <SerializationFixture/Core/Memory.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

namespace detail
{

template <class ArchiveType, typename PointerType, typename KeyType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void native_save(ArchiveType&, PointerType&, KeyType) noexcept { /*pass*/ }

template <class ArchiveType, typename PointerType, typename KeyType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void native_save(ArchiveType& archive, PointerType& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::negation<meta::is_pointer_to_polymorphic<PointerType>>>::value)>
void native_load(ArchiveType&, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& address) noexcept
{
    memory::assign<typename memory::ptr_traits<PointerType>::item>(pointer, address);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer_to_polymorphic<PointerType>>::value)>
void native_load(ArchiveType& archive, PointerType& pointer, typename memory::ptr_traits<PointerType>::void_ptr& address)
{
    auto& registry = archive.registry();

    auto const id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

#endif // SF_NATIVE_SAVE_LOAD_HPP
