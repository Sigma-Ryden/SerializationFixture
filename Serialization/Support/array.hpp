#ifndef SERIALIZATION_SUPPORT_ARRAY_HPP
#define SERIALIZATION_SUPPORT_ARRAY_HPP

#include <type_traits> // true_type, false_type

#include <array> // array

#include "../WriteArchive.hpp"
#include "../ReadArchive.hpp"

namespace serialization
{

namespace meta
{

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_WRITE_ARCHIVE_GENERIC(array, meta::is_std_array<T>::value)
{
    for (const auto& item : array)
        archive & item;

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(array, meta::is_std_array<T>::value)
{
    using size_type  = typename T::size_type;

    size_type vector_size;
    archive & vector_size;

    for (auto& item : array)
        archive & item;

    return archive;
}


} // namespace library

} // namespace serialization

#endif // SERIALIZATION_SUPPORT_ARRAY_HPP
