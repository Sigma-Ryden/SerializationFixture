#ifndef SF_BUILT_IN_DEQUE_HPP
#define SF_BUILT_IN_DEQUE_HPP

#include <deque> // deque

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZATION(save, deque, (template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = deque.size();
    archive & size;

    ::sf::compress::slow(archive, deque);
}

TEMPLATE_SERIALIZATION(load, deque, (template <typename ValueType, typename AllocatorType>), std::deque<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    deque.resize(size);
    ::sf::compress::slow(archive, deque);
}

#endif // SF_BUILT_IN_DEQUE_HPP
