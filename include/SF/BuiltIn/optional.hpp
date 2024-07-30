#ifndef SF_BUILT_IN_OPTIONAL_HPP
#define SF_BUILT_IN_OPTIONAL_HPP

#if __cplusplus >= 201703L

#include <type_traits> // true_type, false_type

#include <optional> // optional

#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_optional : std::false_type {};
template <typename T> struct is_std_optional<std::optional<T>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, optional, ::sf::meta::is_std_optional<T>::value)
{
    auto is_init = optional.has_value();
    archive & is_init;

    if (is_init) archive & optional.value();
}

CONDITIONAL_SERIALIZATION(load, optional, ::sf::meta::is_std_optional<T>::value)
{
    auto is_init = false;
    archive & is_init;

    if (is_init)
    {
        optional.emplace();
        archive & optional.value();
    }
}

#endif // if

#endif // SF_BUILT_IN_OPTIONAL_HPP
