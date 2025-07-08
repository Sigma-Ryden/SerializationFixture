#ifndef SF_BUILT_IN_DEQUE_HPP
#define SF_BUILT_IN_DEQUE_HPP

#include <cstdint> // uint64_t

#include <deque> // deque

#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, deque, (template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
    SERIALIZATION
    (
        std::uint64_t size = deque.size();
        archive & size;

        ::sf::compress::slow(archive, deque);
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, deque, (template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        deque.resize(size);
        ::sf::compress::slow(archive, deque);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_DEQUE_HPP
