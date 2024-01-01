#ifndef SF_BUILT_IN_ARRAY_HPP
#define SF_BUILT_IN_ARRAY_HPP

#include <array> // array

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>
#include <SF/Compress.hpp>

#include <SF/Detail/Meta.hpp> // // is_std_array

namespace sf
{

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, array, meta::is_std_array<T>::value)
{
    compress::zip(archive, array);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_array<T>::value)

#endif // SF_BUILT_IN_ARRAY_HPP
