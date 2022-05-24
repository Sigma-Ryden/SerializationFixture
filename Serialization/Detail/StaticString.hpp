#ifndef SERIALIZATION_STATIC_STRING_HPP
#define SERIALIZATION_STATIC_STRING_HPP

#include "Meta.hpp"

#define STATIC_STRING_BUFFER(text)                                                                      \
    []{                                                                                                 \
        struct string_buffer                                                                            \
        {                                                                                               \
            static constexpr auto data() noexcept -> decltype(text)                                     \
            { return text; }                                                                            \
        };                                                                                              \
        return string_buffer{};                                                                         \
    }()

#define STATIC_STRING(text) (::serialization::prepare(STATIC_STRING_BUFFER(text)))

namespace serialization
{

template <char ...s>
struct static_string {};

namespace detail
{

template <typename S, std::size_t... N>
constexpr static_string<S::data()[N]...> prepare_impl(S, meta::index_sequence<N...>)
{
    return {};
}

} // namespace detail

template <typename S>
constexpr auto prepare(S s) ->
    decltype(detail::prepare_impl(s, meta::make_index_sequence<sizeof(S::data()) - 1>{}))
{
    return detail::prepare_impl(s, meta::make_index_sequence<sizeof(S::data()) - 1>{});
}

} // namespace serialization

#endif // SERIALIZATION_STATIC_STRING_HPP
