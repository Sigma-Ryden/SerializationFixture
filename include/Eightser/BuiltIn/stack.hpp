#ifndef EIGHTSER_BUILT_IN_STACK_HPP
#define EIGHTSER_BUILT_IN_STACK_HPP

#include <stack> // stack

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/BuiltIn/Detail/Meta.hpp>

// default container for stack
#include <Eightser/BuiltIn/deque.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, class ContainerType>), std::stack<ValueType, ContainerType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, stack, (template <typename ValueType, class ContainerType>), std::stack<ValueType, ContainerType>)
    SERIALIZATION
    (
        archive & ::eightser::meta::underlying(stack);
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_STACK_HPP
