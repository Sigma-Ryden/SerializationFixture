#ifndef SF_BUILT_IN_VECTOR_HPP
#define SF_BUILT_IN_VECTOR_HPP

#include <cstdint> // uint64_t

#include <vector> // vector

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, vector, (template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
    SERIALIZATION
    (
        std::uint64_t size = vector.size();
        archive & size;

        ::sf::compress::zip(archive, vector);
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, vector, (template <typename ValueType, typename AllocatorType>), std::vector<ValueType, AllocatorType>)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        vector.resize(size);
        ::sf::compress::zip(archive, vector);
    )
SERIALIZABLE_INIT()


SERIALIZABLE_DECLARATION(std::vector<bool>)
SERIALIZABLE_DECLARATION_INIT()

#endif // SF_BUILT_IN_VECTOR_HPP
