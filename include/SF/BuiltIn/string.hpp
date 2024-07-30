#ifndef SF_BUILT_IN_STRING_HPP
#define SF_BUILT_IN_STRING_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

#include <SF/Core/TypeCore.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename>
struct is_std_basic_string : std::false_type {};

template <typename Char, typename Traitss, typename Alloc>
struct is_std_basic_string<std::basic_string<Char, Traitss, Alloc>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, string, ::sf::meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    ::sf::let::u64 size = string.size();
    archive & size;

    ::sf::compress::zip(archive, string);
}

CONDITIONAL_SERIALIZATION(load, string, ::sf::meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    ::sf::let::u64 size{};
    archive & size;

    string.resize(size);
    ::sf::compress::zip(archive, string);
}

#endif // SF_BUILT_IN_STRING_HPP
