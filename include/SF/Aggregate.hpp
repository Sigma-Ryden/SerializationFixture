#ifndef SF_AGGREGATE_HPP
#define SF_AGGREGATE_HPP

#if __cplusplus >= 201703L

#include <SF/Core/Serialization.hpp>

#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Preprocessor.hpp>

#define SF_AGGREGATE_IMPLEMENTATION_GENERIC(count)                                                      \
    template <class Archive, typename T>                                                                \
    void aggregate_impl(Archive& archive, T& object, std::integral_constant<std::size_t, count>) {      \
        auto& [SF_PLACEHOLDERS(count)] = object;                                                        \
        archive(SF_PLACEHOLDERS(count));                                                                \
    }

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_aggregate
    : all<is_aggregate<T>,
          negation<std::is_union<T>>/*,
          negation<meta::is_has_any_save_mode<T>>,
          negation<meta::is_has_any_save_mode<T>>*/> {};

} // namespace meta

namespace detail
{

template <class Archive, typename T>
void aggregate_impl(Archive& archive, T& object, std::integral_constant<std::size_t, 0>) noexcept { /*pass*/ }

SF_REPEAT(SF_AGGREGATE_IMPLEMENTATION_GENERIC, 64)

} // namespace detail

template <class Archive, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               meta::is_aggregate<T>>::value)>
void aggregate(Archive& archive, T& object)
{
    constexpr auto size = meta::aggregate_size<T>::value;
    detail::aggregate_impl(archive, object, std::integral_constant<std::size_t, size>{});
}

namespace apply
{

template <typename T>
struct aggregate_functor_t : apply_functor_t
{
    T& object;

    aggregate_functor_t(T& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { aggregate(archive, object); }
};

} // namespace apply

template <typename T> apply::aggregate_functor_t<T> aggregate(T& object) noexcept { return { object }; }

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, object, ::sf::meta::is_serializable_aggregate<T>::value)
{
    ::sf::aggregate(archive, object);
}

// clean up
#undef SF_AGGREGATE_IMPLEMENTATION_GENERIC

#endif // if

#endif // SF_AGGREGATE_HPP
