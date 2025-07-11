#ifndef SF_AGGREGATE_HPP
#define SF_AGGREGATE_HPP

#include <SF/Core/Serialization.hpp>

#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Preprocessor.hpp>

#define SF_AGGREGATE_IMPLEMENTATION_GENERIC(count) \
    template <class ArchiveType, typename SerializableType> \
    void aggregate_impl(ArchiveType& archive, SerializableType& object, std::integral_constant<std::size_t, count>) { \
        auto& [SF_PLACEHOLDERS(count)] = object; \
        archive(SF_PLACEHOLDERS(count)); \
    }

namespace sf
{

namespace meta
{

template <typename> struct is_weak_aggregate : std::false_type {};

template <typename AggregateType> struct is_aggregate
    : std::conjunction<std::is_aggregate<AggregateType>,
                       std::negation<is_weak_aggregate<AggregateType>>,
                       std::negation<std::is_array<AggregateType>>> {};

template <typename SerializableType> struct is_serializable_aggregate
    : std::conjunction<is_aggregate<SerializableType>, std::negation<std::is_union<SerializableType>>> {};

} // namespace meta

namespace detail
{

template <class ArchiveType, typename SerializableType>
void aggregate_impl(ArchiveType& archive, SerializableType& object, std::integral_constant<std::size_t, 0>) noexcept { /*pass*/ }

SF_REPEAT(SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       meta::is_aggregate<SerializableType>>::value)>
void aggregate(ArchiveType& archive, SerializableType& object)
{
    constexpr auto N = meta::aggregate_size<SerializableType>::size();
    detail::aggregate_impl(archive, object, std::integral_constant<std::size_t, N>{});
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


CONDITIONAL_SERIALIZABLE_DECLARATION(::sf::meta::is_serializable_aggregate<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, object, ::sf::meta::is_serializable_aggregate<S>::value)
    SERIALIZATION
    (
        ::sf::aggregate(archive, object);
    )
SERIALIZABLE_INIT()

// clean up
#undef SF_AGGREGATE_IMPLEMENTATION_GENERIC

#include <array> // array

namespace sf
{

namespace meta
{

// write own specializations to control aggregate type serialization
template <typename ValueType, std::size_t SizeValue>
struct is_weak_aggregate<std::array<ValueType, SizeValue>> : std::true_type {};

} // namespace meta

} // namespace sf

#endif // SF_AGGREGATE_HPP
