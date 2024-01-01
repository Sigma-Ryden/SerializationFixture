#ifndef SF_BUILT_IN_SHARED_PTR_HPP
#define SF_BUILT_IN_SHARED_PTR_HPP

#include <memory> // shared_ptr

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

#include <SF/DataTrack.hpp>

#include <SF/Detail/Meta.hpp> // is_std_shared_ptr

namespace sf
{

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, shared_ptr, meta::is_std_shared_ptr<T>::value)
{
    tracking::track(archive, shared_ptr);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_shared_ptr<T>::value)

#endif // SF_BUILT_IN_SHARED_PTR_HPP
