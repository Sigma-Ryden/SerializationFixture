#ifndef SF_BUILT_IN_LIST_HPP
#define SF_BUILT_IN_LIST_HPP

#include <list> // list

#include <SerializationFixture/Core/TypeCore.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZATION(save, list, (template <typename ValueType, typename AllocatorType>), std::list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size = list.size();
    archive & size;

    ::sf::compress::slow(archive, list);
}

TEMPLATE_SERIALIZATION(load, list, (template <typename ValueType, typename AllocatorType>), std::list<ValueType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    list.resize(size);
    ::sf::compress::slow(archive, list);
}

#endif // SF_BUILT_IN_LIST_HPP
