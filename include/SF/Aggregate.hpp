#ifndef SF_AGGREGATE_HPP
#define SF_AGGREGATE_HPP

#if __cplusplus >= 201703L

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/ExternSerialization.hpp>
#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Preprocessor.hpp>

#define _SF_AGGREGATE_IMPLEMENTATION_GENERIC(count)                                                     \
    template <class Archive, typename T>                                                                \
    void aggregate_implementation(Archive& archive, T& object, meta::dispatch<count>) {                 \
        auto& [SFPLACEHOLDERS(count)] = object;                                                         \
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
void aggregate_implementation(Archive& archive, T& object, meta::dispatch<0>) noexcept { /*pass*/ }

SFREPEAT(_SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class Archive, typename T,
          SFREQUIRE(meta::is_aggregate<T>::value)>
void aggregate(Archive& archive, T& object)
{
    constexpr auto size = meta::aggregate_size<T>::value;
    detail::aggregate_implementation(archive, object, meta::dispatch<size>{});
}

inline namespace common
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, object, meta::is_serializable_aggregate<T>::value)
{
    aggregate(archive, object);
    return archive;
}

} // inline namespace common

namespace apply
{

template <typename T>
struct AggregateFunctor : ApplyFunctor
{
    T& object;

    AggregateFunctor(T& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { aggregate(archive, object); }
};

} // namespace apply

template <typename T> apply::AggregateFunctor<T> aggregate(T& object) noexcept { return { object }; }

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_serializable_aggregate<T>::value)

// clean up
#undef _SF_AGGREGATE_IMPLEMENTATION_GENERIC

#endif // if

#endif // SF_AGGREGATE_HPP
