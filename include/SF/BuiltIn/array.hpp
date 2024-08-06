#ifndef SF_BUILT_IN_ARRAY_HPP
#define SF_BUILT_IN_ARRAY_HPP

#include <array> // array

#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZATION(saveload, array, (template <typename ValueType, std::size_t BitsetSize>), std::array<ValueType, BitsetSize>)
{
    ::sf::compress::zip(archive, array);
}

#endif // SF_BUILT_IN_ARRAY_HPP
