#ifndef SF_BUILT_IN_FORWARD_LIST_HPP
#define SF_BUILT_IN_FORWARD_LIST_HPP

#include <forward_list> // forward_list

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZATION(save, forward_list,
    (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = std::distance(forward_list.begin(), forward_list.end());
    archive & size;

    ::sf::compress::slow(archive, forward_list);
}

TEMPLATE_SERIALIZATION(load, forward_list,
    (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    forward_list.resize(size);
    ::sf::compress::slow(archive, forward_list);
}

#endif // SF_BUILT_IN_FORWARD_LIST_HPP
