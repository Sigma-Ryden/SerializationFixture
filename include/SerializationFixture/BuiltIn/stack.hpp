#ifndef SF_BUILT_IN_STACK_HPP
#define SF_BUILT_IN_STACK_HPP

#include <stack> // stack

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/BuiltIn/Detail/Meta.hpp>

// default container for stack
#include <SerializationFixture/BuiltIn/deque.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, class ContainerType>), std::stack<ValueType, ContainerType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, stack, (template <typename ValueType, class ContainerType>), std::stack<ValueType, ContainerType>)
    SERIALIZATION
    (
        archive & ::sf::meta::underlying(stack);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_STACK_HPP
