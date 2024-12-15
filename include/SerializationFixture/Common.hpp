#ifndef SF_COMMON_HPP
#define SF_COMMON_HPP

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/OArchive.hpp>
#include <SerializationFixture/IArchive.hpp>

#include <SerializationFixture/Binary.hpp>
#include <SerializationFixture/DataTrack.hpp>
#include <SerializationFixture/Compress.hpp>

CONDITIONAL_SERIALIZATION(saveload, number, std::is_arithmetic<S>::value)
{
    ::sf::binary(archive, number);
}

CONDITIONAL_SERIALIZATION(save, enumerator, std::is_enum<S>::value)
{
    auto value = static_cast<typename std::underlying_type<S>::type>(enumerator);
    archive & value;
}

CONDITIONAL_SERIALIZATION(load, enumerator, std::is_enum<S>::value)
{
    typename std::underlying_type<S>::type buff{};
    archive & buff;

    enumerator = static_cast<S>(buff);
}

CONDITIONAL_SERIALIZATION(saveload, array, std::is_array<S>::value)
{
    ::sf::compress::zip(archive, array);
}

CONDITIONAL_SERIALIZATION(saveload, pointer, ::sf::meta::all<std::is_pointer<S>, ::sf::meta::is_pointer_to_any<S>>::value)
{
#ifdef SF_PTRTRACK_DISABLE
    ::sf::raw(archive, pointer);
#else
    ::sf::track(archive, pointer);
#endif // SF_PTRTRACK_DISABLE
}

#endif // SF_COMMON_HPP
