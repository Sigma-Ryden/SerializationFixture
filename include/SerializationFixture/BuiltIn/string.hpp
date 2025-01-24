#ifndef SF_BUILT_IN_STRING_HPP
#define SF_BUILT_IN_STRING_HPP

#include <cstdint> // uint64_t

#include <string> // basic_string

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION
(
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>
)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE
(
    save, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>
)
    SERIALIZATION
    (
        std::uint64_t size = string.size();
        archive & size;

        ::sf::compress::zip(archive, string);
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE
(
    load, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>
)
    SERIALIZATION
    (
        std::uint64_t size{};
        archive & size;

        string.resize(size);
        ::sf::compress::zip(archive, string);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_STRING_HPP
