#ifndef SF_BUILT_IN_FORWARD_LIST_HPP
#define SF_BUILT_IN_FORWARD_LIST_HPP

#include <cstdint> // uint64_t

#include <forward_list> // forward_list

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZATION(save, forward_list,
    (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
{
    std::uint64_t size = std::distance(forward_list.begin(), forward_list.end());
    archive & size;

    ::sf::compress::slow(archive, forward_list);
}

TEMPLATE_SERIALIZATION(load, forward_list,
    (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
{
    std::uint64_t size{};
    archive & size;

    forward_list.resize(size);
    ::sf::compress::slow(archive, forward_list);
}

#endif // SF_BUILT_IN_FORWARD_LIST_HPP
