#ifndef SF_UNION_HPP
#define SF_UNION_HPP

#include <SerializationFixture/Core/Serialization.hpp>
#include <SerializationFixture/Binary.hpp>

CONDITIONAL_SERIALIZATION(saveload, data, std::is_union<S>::value)
{
    ::sf::binary(archive, data);
}

#endif // SF_UNION_HPP
