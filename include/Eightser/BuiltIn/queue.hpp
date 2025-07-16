#ifndef EIGHTSER_BUILT_IN_QUEUE_HPP
#define EIGHTSER_BUILT_IN_QUEUE_HPP

#include <queue> // queue

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/BuiltIn/Detail/Meta.hpp>

// default container for queue
#include <Eightser/BuiltIn/deque.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, class ContainerType>), std::queue<ValueType, ContainerType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, queue, (template <typename ValueType, class ContainerType>), std::queue<ValueType, ContainerType>)
    SERIALIZATION
    (
        archive & ::eightser::meta::underlying(queue);
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_QUEUE_HPP
