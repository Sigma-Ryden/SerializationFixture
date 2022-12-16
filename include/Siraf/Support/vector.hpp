#ifndef SIRAF_SUPPORT_VECTOR_HPP
#define SIRAF_SUPPORT_VECTOR_HPP

#include <type_traits> // true_type, false_type

#include <vector> // vector

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/Compress.hpp>

#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Utility.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

CONDITIONAL_SAVE_SERIALIZABLE_TYPE(vector, meta::is_std_vector<T>::value)
{
    let::u64 size = vector.size();
    archive & size;

    compress::zip(archive, vector);

    return archive;
}

CONDITIONAL_LOAD_SERIALIZABLE_TYPE(vector, meta::is_std_vector<T>::value)
{
    let::u64 size;
    archive & size;

    vector.resize(size);
    compress::zip(archive, vector);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_vector<T>::value)

#endif // SIRAF_SUPPORT_VECTOR_HPP
