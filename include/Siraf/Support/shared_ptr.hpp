#ifndef SIRAF_SUPPORT_SHARED_PTR_HPP
#define SIRAF_SUPPORT_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>
#include <Siraf/UnifiedData.hpp>

#include <Siraf/TypeRegistry.hpp>

#include <Siraf/DataTrack.hpp>

#include <Siraf/Detail/Meta.hpp> // is_std_shared_ptr

namespace siraf
{

inline namespace library
{

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(shared_ptr, meta::is_std_shared_ptr<T>::value)
{
    tracking::track(archive, shared_ptr);
    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_shared_ptr<T>::value)

#endif // SIRAF_SUPPORT_SHARED_PTR_HPP
