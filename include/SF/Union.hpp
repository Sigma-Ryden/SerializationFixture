#ifndef SF_UNION_HPP
#define SF_UNION_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Binary.hpp>

CONDITIONAL_SERIALIZATION(saveload, data, std::is_union<T>::value)
{
    ::sf::binary(archive, data);
}

#endif // SF_UNION_HPP
