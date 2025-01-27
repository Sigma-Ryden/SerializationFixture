#ifndef SF_COMMON_HPP
#define SF_COMMON_HPP

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/OArchive.hpp>
#include <SerializationFixture/IArchive.hpp>

#include <SerializationFixture/Binary.hpp>
#include <SerializationFixture/DataTrack.hpp>
#include <SerializationFixture/Compress.hpp>


CONDITIONAL_SERIALIZABLE_DECLARATION(std::is_arithmetic<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, number, std::is_arithmetic<S>::value)
    SERIALIZATION
    (
        ::sf::binary(archive, number);
    )
SERIALIZABLE_INIT()


CONDITIONAL_SERIALIZABLE_DECLARATION(std::is_enum<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(save, enumerator, std::is_enum<S>::value)
    SERIALIZATION
    (
        auto value = static_cast<typename std::underlying_type<S>::type>(enumerator);
        archive & value;
    )
SERIALIZABLE_INIT()

CONDITIONAL_SERIALIZABLE(load, enumerator, std::is_enum<S>::value)
    SERIALIZATION
    (
        typename std::underlying_type<S>::type buff{};
        archive & buff;

        enumerator = static_cast<S>(buff);
    )
SERIALIZABLE_INIT()


CONDITIONAL_SERIALIZABLE_DECLARATION(std::is_array<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, array, std::is_array<S>::value)
    SERIALIZATION
    (
        ::sf::compress::zip(archive, array);
    )
SERIALIZABLE_INIT()


template <typename SerializableType>
struct xxsf_is_pointer
    : std::conjunction<std::is_pointer<SerializableType>, ::sf::meta::is_pointer_to_any<SerializableType>> {};

CONDITIONAL_SERIALIZABLE_DECLARATION(xxsf_is_pointer<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, pointer, xxsf_is_pointer<S>::value)
    SERIALIZATION
    (
    #ifdef SF_PTRTRACK_DISABLE
        ::sf::raw(archive, pointer);
    #else
        ::sf::track(archive, pointer);
    #endif // SF_PTRTRACK_DISABLE
    )
SERIALIZABLE_INIT()

#endif // SF_COMMON_HPP
