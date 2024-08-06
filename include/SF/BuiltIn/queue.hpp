#ifndef SF_BUILT_IN_QUEUE_HPP
#define SF_BUILT_IN_QUEUE_HPP

#include <queue> // queue

#include <SF/Core/Serialization.hpp>

#include <SF/BuiltIn/Detail/Meta.hpp>

// default container for queue
#include <SF/BuiltIn/deque.hpp>

TEMPLATE_SERIALIZATION(saveload, queue,
    (template <typename ValueType, class ContainerType>), std::queue<ValueType, ContainerType>)
{
    archive & ::sf::meta::underlying(queue);
}

#endif // SF_BUILT_IN_QUEUE_HPP
