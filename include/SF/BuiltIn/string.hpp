#ifndef SF_BUILT_IN_STRING_HPP
#define SF_BUILT_IN_STRING_HPP

#include <string> // basic_string

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

TEMPLATE_SERIALIZATION(save, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>)
{
    ::sf::let::u64 size = string.size();
    archive & size;

    ::sf::compress::zip(archive, string);
}

TEMPLATE_SERIALIZATION(load, string,
    (template <typename CharType, typename TraitsType, typename AllocatorType>),
    std::basic_string<CharType, TraitsType, AllocatorType>)
{
    ::sf::let::u64 size{};
    archive & size;

    string.resize(size);
    ::sf::compress::zip(archive, string);
}

#endif // SF_BUILT_IN_STRING_HPP
