#ifndef SF_BUILT_IN_ARRAY_HPP
#define SF_BUILT_IN_ARRAY_HPP

//#include <array> // array

#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename ValueType, std::size_t SizeValue>), std::array<ValueType, SizeValue>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, array, (template <typename ValueType, std::size_t SizeValue>), std::array<ValueType, SizeValue>)
    SERIALIZATION
    (
        ::sf::compress::zip(archive, array);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_ARRAY_HPP
