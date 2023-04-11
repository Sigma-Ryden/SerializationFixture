#ifndef SF_AGGREGATE_HPP
#define SF_AGGREGATE_HPP

#if __cplusplus >= 201703L

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Utility/Expanding.hpp>

#include <SF/ExternSerialization.hpp>

// default pack type
#include <SF/Support/tuple.hpp>

namespace sf
{

namespace meta
{

template <typename T> constexpr bool is_serializable_aggregate() noexcept
{
    return meta::is_aggregate<T>() and
       not ::Serialization::has_load_mode<T>() and
       not ::Serialization::has_load_mode<T>();
}

} // namespace meta

inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, aggregate, meta::is_serializable_aggregate<T>())
{
    auto forward_tuple = utility::expand(aggregate);
    archive & forward_tuple;

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_aggregate<T>())

#endif // if

#endif // SF_AGGREGATE_HPP
