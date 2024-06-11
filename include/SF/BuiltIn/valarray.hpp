#ifndef SF_BUILT_IN_VALARRAY_HPP
#define SF_BUILT_IN_VALARRAY_HPP

#include <type_traits> // true_type, false_type

#include <valarray> // valarray

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/ExternSerialization.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_valarray : std::false_type {};
template <typename T> struct is_std_valarray<std::valarray<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, valarray, meta::is_std_valarray<T>::value)
{
    let::u64 size = valarray.size();
    archive & size;

    compress::zip(archive, valarray);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, valarray, meta::is_std_valarray<T>::value)
{
    let::u64 size{};
    archive & size;

    valarray.resize(size);
    compress::zip(archive, valarray);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_valarray<T>::value)

#endif // SF_BUILT_IN_VALARRAY_HPP
