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

template <class Archive, typename T, typename KeyType,
          SF_REQUIRE(meta::all<meta::is_oarchive<Archive>,
                               meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_save(Archive& archive, T& pointer, KeyType track_key) noexcept { /*pass*/ }

template <class Archive, typename T, typename KeyType,
          SF_REQUIRE(meta::all<meta::is_oarchive<Archive>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void native_save(Archive& archive, T& pointer, KeyType track_key)
{
    archive.registry().save_key(archive, pointer); // write class info
}

template <class Archive, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<Archive>,
                               meta::negation<meta::is_pointer_to_polymorphic<T>>>::value)>
void native_load(Archive& archive, T& pointer, memory::void_ptr<T>& address) noexcept
{
    memory::assign<meta::dereference<T>>(pointer, address);
}

template <class Archive, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<Archive>,
                               meta::is_pointer_to_polymorphic<T>>::value)>
void native_load(Archive& archive, T& pointer, memory::void_ptr<T>& address)
{
    auto& registry = archive.registry();

    auto id = registry.load_key(archive, pointer);
    registry.assign(pointer, address, id);
}

} // namespace detail

} // namespace sf

#endif // SF_NATIVE_SAVE_LOAD_HPP
