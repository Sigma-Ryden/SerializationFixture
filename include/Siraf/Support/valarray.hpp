#ifndef SIRAF_SUPPORT_VALARRAY_HPP
#define SIRAF_SUPPORT_VALARRAY_HPP

#include <type_traits> // true_type, false_type

#include <valarray> // valarray

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/Compress.hpp>
#include <Siraf/TypeRegistry.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_valarray : std::false_type {};
template <typename T>
struct is_std_valarray<std::valarray<T>> : std::true_type {};

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
    let::u64 size;
    archive & size;

    valarray.resize(size);
    compress::zip(archive, valarray);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_valarray<T>::value)

#endif // SIRAF_SUPPORT_VALARRAY_HPP
