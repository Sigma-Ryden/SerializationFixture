#ifndef SF_BUILT_IN_ARRAY_HPP
#define SF_BUILT_IN_ARRAY_HPP

#include <array> // array

#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

#include <SF/Detail/Meta.hpp> // // is_std_array

CONDITIONAL_SERIALIZATION(saveload, array, ::sf::meta::is_std_array<T>::value)
{
    ::sf::compress::zip(archive, array);
}

#endif // SF_BUILT_IN_ARRAY_HPP
