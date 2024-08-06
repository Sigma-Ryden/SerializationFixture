#ifndef SF_BUILT_IN_OPTIONAL_HPP
#define SF_BUILT_IN_OPTIONAL_HPP

#if __cplusplus >= 201703L

#include <optional> // optional

#include <SF/Core/Serialization.hpp>

TEMPLATE_SERIALIZATION(save, optional, (template <typename ValueType>), std::optional<ValueType>)
{
    auto is_init = optional.has_value();
    archive & is_init;

    if (is_init) archive & optional.value();
}

TEMPLATE_SERIALIZATION(load, optional, (template <typename ValueType>), std::optional<ValueType>)
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
