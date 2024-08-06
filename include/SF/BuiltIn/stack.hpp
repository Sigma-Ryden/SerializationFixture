#ifndef SF_BUILT_IN_STACK_HPP
#define SF_BUILT_IN_STACK_HPP

#include <stack> // stack

#include <SF/Core/Serialization.hpp>

#include <SF/BuiltIn/Detail/Meta.hpp>

// default container for stack
#include <SF/BuiltIn/deque.hpp>

TEMPLATE_SERIALIZATION(saveload, stack,
    (template <typename ValueType, class ContainerType>), std::stack<ValueType, ContainerType>)
{
    archive & ::sf::meta::underlying(stack);
}

#endif // SF_BUILT_IN_STACK_HPP
