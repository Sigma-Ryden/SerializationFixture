#ifndef SF_BUILT_IN_OPTIONAL_HPP
#define SF_BUILT_IN_OPTIONAL_HPP

#if __cplusplus >= 201703L

#include <optional> // optional

#include <SerializationFixture/Core/Serialization.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename ValueType>, std::optional<ValueType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, optional, template <typename ValueType>, std::optional<ValueType>)
    SERIALIZATION
    (
        auto is_init = optional.has_value();
        archive & is_init;

        if (is_init) archive & optional.value();
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, optional, template <typename ValueType>, std::optional<ValueType>)
    SERIALIZATION
    (
        auto is_init = false;
        archive & is_init;

        if (is_init)
        {
            optional.emplace();
            archive & optional.value();
        }
    )
SERIALIZABLE_INIT()

#endif // if

#endif // SF_BUILT_IN_OPTIONAL_HPP
