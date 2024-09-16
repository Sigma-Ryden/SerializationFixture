#ifndef SF_BUILT_IN_ARRAY_HPP
#define SF_BUILT_IN_ARRAY_HPP

#include <array> // array

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZATION(saveload, array,
    (template <typename ValueType, std::size_t SizeValue>), std::array<ValueType, SizeValue>)
{
    ::sf::compress::zip(archive, array);
}

#endif // SF_BUILT_IN_ARRAY_HPP
