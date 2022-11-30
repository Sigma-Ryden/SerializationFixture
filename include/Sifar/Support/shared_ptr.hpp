#ifndef SIFAR_SUPPORT_SHARED_PTR_HPP
#define SIFAR_SUPPORT_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/UnifiedData.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/DataTrack.hpp>

#include <Sifar/Detail/Meta.hpp> // is_std_shared_ptr

namespace sifar
{

inline namespace library
{

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(shared_ptr, meta::is_std_shared_ptr<T>::value)
{
    tracking::track(archive, shared_ptr);
    return archive;
}

} // inline namespace library

} // namespace sifar

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_shared_ptr<T>::value)

#endif // SIFAR_SUPPORT_SHARED_PTR_HPP
