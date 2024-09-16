#ifndef SF_BUILT_IN_VALARRAY_HPP
#define SF_BUILT_IN_VALARRAY_HPP

#include <valarray> // valarray

#include <SerializationFixture/Core/TypeCore.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

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
