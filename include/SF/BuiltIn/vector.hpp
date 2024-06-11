#ifndef SF_BUILT_IN_VECTOR_HPP
#define SF_BUILT_IN_VECTOR_HPP

#include <type_traits> // true_type, false_type

#include <vector> // vector

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/ExternSerialization.hpp>
#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_vector : std::false_type {};
template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, vector, meta::is_std_vector<T>::value)
{
    let::u64 size = vector.size();
    archive & size;

    compress::zip(archive, vector);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, vector, meta::is_std_vector<T>::value)
{
    let::u64 size{};
    archive & size;

    vector.resize(size);
    compress::zip(archive, vector);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_vector<T>::value)
TYPE_REGISTRY(std::vector<bool>)

namespace sf
{

inline namespace library
{

// slow implementation
EXTERN_SERIALIZATION(Save, vector, std::vector<bool>)
{
    let::u64 size = vector.size();
    archive & size;

    for(auto item:vector)
    {
        bool boolean = item;
        archive & boolean;
    }

    return archive;
}

EXTERN_SERIALIZATION(Load, vector, std::vector<bool>)
{
    let::u64 size{};
    archive & size;

    vector.resize(size);

    for(auto item/*bit_reference*/:vector)
    {
        bool boolean{};
        archive & boolean;
        item = boolean;
    }

    return archive;
}

} // inline namespace library

} // namespace sf

#endif // SF_BUILT_IN_VECTOR_HPP
