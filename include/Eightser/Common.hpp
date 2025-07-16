#ifndef EIGHTSER_COMMON_HPP
#define EIGHTSER_COMMON_HPP

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/OArchive.hpp>
#include <Eightser/IArchive.hpp>

#include <Eightser/Binary.hpp>
#include <Eightser/DataTrack.hpp>
#include <Eightser/Compress.hpp>


CONDITIONAL_SERIALIZABLE_DECLARATION(std::is_arithmetic<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, number, std::is_arithmetic<S>::value)
    SERIALIZATION
    (
        ::eightser::binary(archive, number);
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
        ::eightser::compress::zip(archive, array);
    )
SERIALIZABLE_INIT()


template <typename SerializableType>
struct xxeightser_is_pointer
    : std::conjunction<std::is_pointer<SerializableType>, ::eightser::meta::is_pointer_to_any<SerializableType>> {};

CONDITIONAL_SERIALIZABLE_DECLARATION(xxeightser_is_pointer<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, pointer, xxeightser_is_pointer<S>::value)
    SERIALIZATION
    (
    #ifdef EIGHTSER_PTRTRACK_DISABLE
        ::eightser::raw(archive, pointer);
    #else
        ::eightser::track(archive, pointer);
    #endif // EIGHTSER_PTRTRACK_DISABLE
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_COMMON_HPP
