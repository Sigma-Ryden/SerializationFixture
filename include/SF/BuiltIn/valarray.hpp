#ifndef SF_BUILT_IN_VALARRAY_HPP
#define SF_BUILT_IN_VALARRAY_HPP

#include <valarray> // valarray

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZATION(save, valarray, template <typename ValueType>, std::valarray<ValueType>)
{
    ::sf::let::u64 size = valarray.size();
    archive & size;

    ::sf::compress::zip(archive, valarray);
}

TEMPLATE_SERIALIZATION(load, valarray, template <typename ValueType>, std::valarray<ValueType>)
{
    ::sf::let::u64 size{};
    archive & size;

    valarray.resize(size);
    ::sf::compress::zip(archive, valarray);
}

#endif // SF_BUILT_IN_VALARRAY_HPP
