#ifndef SF_AGGREGATE_HPP
#define SF_AGGREGATE_HPP

#if __cplusplus >= 201703L

#include <SF/Core/Serialization.hpp>

#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Preprocessor.hpp>

#define SF_AGGREGATE_IMPLEMENTATION_GENERIC(count)                                                      \
    template <class ArchiveType, typename SerializableType>                                             \
    void aggregate_impl(ArchiveType& archive, SerializableType& object,                                 \
                        std::integral_constant<std::size_t, count>) {                                   \
        auto& [SF_PLACEHOLDERS(count)] = object;                                                        \
        archive(SF_PLACEHOLDERS(count));                                                                \
    }

namespace sf
{

namespace meta
{

template <typename SerializableType> struct is_serializable_aggregate
    : all<is_aggregate<SerializableType>, negation<std::is_union<SerializableType>>> {};

} // namespace meta

namespace detail
{

template <class ArchiveType, typename SerializableType>
void aggregate_impl(ArchiveType& archive, SerializableType& object, std::integral_constant<std::size_t, 0>) noexcept { /*pass*/ }

SF_REPEAT(SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<ArchiveType>,
                                meta::is_aggregate<T>>::value)>
void aggregate(ArchiveType& archive, SerializableType& object)
{
    constexpr auto size = meta::aggregate_size<SerializableType>::value;
    detail::aggregate_impl(archive, object, std::integral_constant<std::size_t, size>{});
}

namespace apply
{

template <typename SerializableType>
struct aggregate_functor_t : apply_functor_t
{
    SerializableType& object;

    aggregate_functor_t(SerializableType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { aggregate(archive, object); }
};

} // namespace apply

template <typename SerializableType>
apply::aggregate_functor_t<SerializableType> aggregate(SerializableType& object) noexcept { return { object }; }

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, object, ::sf::meta::is_serializable_aggregate<S>::value)
{
    ::sf::aggregate(archive, object);
}

// clean up
#undef SF_AGGREGATE_IMPLEMENTATION_GENERIC

#endif // if

#endif // SF_AGGREGATE_HPP
