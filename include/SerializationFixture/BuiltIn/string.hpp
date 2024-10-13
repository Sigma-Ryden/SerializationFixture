#ifndef SF_BUILT_IN_STRING_HPP
#define SF_BUILT_IN_STRING_HPP

#include <cstdint> // uint64_t

#include <string> // basic_string

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Compress.hpp>

TEMPLATE_SERIALIZATION(save, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>)
{
    std::uint64_t size = string.size();
    archive & size;

    ::sf::compress::zip(archive, string);
}

TEMPLATE_SERIALIZATION(load, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>)
{
    std::uint64_t size{};
    archive & size;

    string.resize(size);
    ::sf::compress::zip(archive, string);
}

#endif // SF_BUILT_IN_STRING_HPP
