#ifndef SF_BUILT_IN_PRIORITY_QUEUE_HPP
#define SF_BUILT_IN_PRIORITY_QUEUE_HPP

#include <queue> // priority_queue

#include <SF/Core/Serialization.hpp>

#include <SF/BuiltIn/Detail/Meta.hpp>

// default container for priority_queue
#include <SF/BuiltIn/vector.hpp>

TEMPLATE_SERIALIZATION(saveload, priority_queue,
    (template <typename ValueType, class ContainerType>), std::priority_queue<ValueType, ContainerType>)
{
    archive & ::sf::meta::underlying(priority_queue);
}

#endif // SF_BUILT_IN_PRIORITY_QUEUE_HPP
