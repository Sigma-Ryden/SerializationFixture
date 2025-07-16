#ifndef EIGHTSER_BUILT_IN_STRING_HPP
#define EIGHTSER_BUILT_IN_STRING_HPP

#include <cstdint> // uint64_t

#include <string> // basic_string

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Compress.hpp>

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

        ::eightser::compress::zip(archive, string);
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
        ::eightser::compress::zip(archive, string);
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_STRING_HPP
