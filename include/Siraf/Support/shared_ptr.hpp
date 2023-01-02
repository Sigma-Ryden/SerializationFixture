#ifndef SIRAF_SUPPORT_SHARED_PTR_HPP
#define SIRAF_SUPPORT_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>
#include <Siraf/DataTrack.hpp>

#include <Siraf/Detail/Meta.hpp> // is_std_shared_ptr

namespace siraf
{

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, shared_ptr, meta::is_std_shared_ptr<T>::value)
{
    tracking::track(archive, shared_ptr);
    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_shared_ptr<T>::value)

#endif // SIRAF_SUPPORT_SHARED_PTR_HPP
