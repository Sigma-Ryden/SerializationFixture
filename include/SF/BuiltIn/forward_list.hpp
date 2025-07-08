#ifndef SF_BUILT_IN_FORWARD_LIST_HPP
#define SF_BUILT_IN_FORWARD_LIST_HPP

#include <cstdint> // uint64_t

#include <forward_list> // forward_list

#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, forward_list, (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
    SERIALIZATION
    (
        std::uint64_t size = std::distance(forward_list.begin(), forward_list.end());
        archive & size;

        ::sf::compress::slow(archive, forward_list);
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, forward_list, (template <typename ValueType, typename AllocatorType>), std::forward_list<ValueType, AllocatorType>)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        forward_list.resize(size);
        ::sf::compress::slow(archive, forward_list);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_FORWARD_LIST_HPP
