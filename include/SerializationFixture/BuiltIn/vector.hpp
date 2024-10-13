#ifndef SF_BUILT_IN_VECTOR_HPP
#define SF_BUILT_IN_VECTOR_HPP

#include <cstdint> // uint64_t

#include <vector> // vector

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZATION(save, vector,
    (template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
{
    std::uint64_t size = vector.size();
    archive & size;

    ::sf::compress::zip(archive, vector);
}

TEMPLATE_SERIALIZATION(load, vector,
    (template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
{
    std::uint64_t size{};
    archive & size;

    vector.resize(size);
    ::sf::compress::zip(archive, vector);
}

// slow impl
SERIALIZATION(save, vector, std::vector<bool>)
{
    std::uint64_t size = vector.size();
    archive & size;

    for(auto item:vector)
    {
        bool boolean = item;
        archive & boolean;
    }
}

SERIALIZATION(load, vector, std::vector<bool>)
{
    std::uint64_t size{};
    archive & size;

    vector.resize(size);

    for(auto item/*bit_reference*/:vector)
    {
        bool boolean{};
        archive & boolean;
        item = boolean;
    }
}

#endif // SF_BUILT_IN_VECTOR_HPP
