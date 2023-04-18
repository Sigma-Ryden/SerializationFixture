#ifndef SF_AGGREGATE_HPP
#define SF_AGGREGATE_HPP

#if __cplusplus >= 201703L

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/ExternSerialization.hpp>

#include <SF/Detail/Preprocessor.hpp>

#define _SF_AGGREGATE_SERIALIZATION_GENERIC(count)                                                      \
    template <class Archive, typename T>                                                                \
    void aggregate_serialization(Archive& archive, T& aggregate, meta::dispatch<count>) {               \
        auto& [SFPLACEHOLDERS(count)] = aggregate;                                                      \
        archive(SFPLACEHOLDERS(count));                                                                 \
    }

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_aggregate
    : all<is_aggregate<T>,
          negation<::Serialization::has_save_mode<T>>,
          negation<::Serialization::has_save_mode<T>>> {};

} // namespace meta

namespace detail
{

template <class Archive, typename T>
void aggregate_serialization(Archive& archive, T& aggregate, meta::dispatch<0>) { /*pass*/ }

SFREPEAT(_SF_AGGREGATE_SERIALIZATION_GENERIC, 64)

} // namespace detail

inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, aggregate, meta::is_serializable_aggregate<T>::value)
{
    constexpr auto size = meta::aggregate_size<T>();
    detail::aggregate_serialization(archive, aggregate, meta::dispatch<size>{});

    return archive;
}

} // inline namespace common

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_aggregate<T>::value)

#undef _SF_AGGREGATE_SERIALIZATION_GENERIC

#endif // if

#endif // SF_AGGREGATE_HPP
