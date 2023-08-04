#ifndef SF_UNION_HPP
#define SF_UNION_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_union
    : all<std::is_union<T>,
          negation<::Serialization::has_save_mode<T>>,
          negation<::Serialization::has_save_mode<T>>> {};

} // namespace meta

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, data, meta::is_serializable_union<T>::value)
{
    binary(archive, data);
    return archive;
}

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_union<T>::value)

#endif // SF_UNION_HPP
