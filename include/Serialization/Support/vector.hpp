#ifndef SERIALIZATION_SUPPORT_VECTOR_HPP
#define SERIALIZATION_SUPPORT_VECTOR_HPP

#include <type_traits> // true_type, false_type

#include <vector> // vector

#include <Serialization/WriteArchive.hpp>
#include <Serialization/ReadArchive.hpp>

#include <Serialization/TypeRegistry.hpp>

namespace serialization
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_WRITE_ARCHIVE_GENERIC(vector, meta::is_std_vector<T>::value)
{
    const auto size = vector.size();

    archive & size;
    for (const auto& item : vector)
        archive & item;

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(vector, meta::is_std_vector<T>::value)
{
    using size_type  = typename T::size_type;

    size_type vector_size;
    archive & vector_size;

    vector.resize(vector_size);

    for (auto& item : vector)
        archive & item;

    return archive;
}

} // namespace library

} // namespace serialization

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_vector<T>::value)

#endif // SERIALIZATION_SUPPORT_VECTOR_HPP
