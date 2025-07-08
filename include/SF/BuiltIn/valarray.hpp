#ifndef SF_BUILT_IN_VALARRAY_HPP
#define SF_BUILT_IN_VALARRAY_HPP

#include <cstdint> // uint64_t

#include <valarray> // valarray

#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename ValueType>, std::valarray<ValueType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, valarray, template <typename ValueType>, std::valarray<ValueType>)
    SERIALIZATION
    (
        std::uint64_t size = valarray.size();
        archive & size;

        ::sf::compress::zip(archive, valarray);
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, valarray, template <typename ValueType>, std::valarray<ValueType>)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        valarray.resize(size);
        ::sf::compress::zip(archive, valarray);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_VALARRAY_HPP
