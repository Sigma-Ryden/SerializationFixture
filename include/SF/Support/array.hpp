#ifndef SF_SUPPORT_ARRAY_HPP
#define SF_SUPPORT_ARRAY_HPP

#include <type_traits> // true_type, false_type

#include <array> // array

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>
#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

} // namespace meta

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

#endif // SF_SUPPORT_ARRAY_HPP
