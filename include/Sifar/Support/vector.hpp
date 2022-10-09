#ifndef SIFAR_SUPPORT_VECTOR_HPP
#define SIFAR_SUPPORT_VECTOR_HPP

#include <type_traits> // true_type, false_type

#include <vector> // vector

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Utility.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(vector, meta::is_std_vector<T>::value)
{
    let::u64 size = vector.size();
    archive & size;

    for (const auto& item : vector)
        archive & item;

    return archive;
}

SERIALIZATION_LOAD_DATA(vector, meta::is_std_vector<T>::value)
{
    let::u64 size;
    archive & size;

    vector.resize(size);

    for (auto& item : vector)
        archive & item;

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_vector<T>::value)

#endif // SIFAR_SUPPORT_VECTOR_HPP
