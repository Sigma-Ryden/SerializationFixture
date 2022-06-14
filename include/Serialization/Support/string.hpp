#ifndef SERIALIZATION_SUPPORT_STRING_HPP
#define SERIALIZATION_SUPPORT_STRING_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

#include <Serialization/WriteArchive.hpp>
#include <Serialization/ReadArchive.hpp>

namespace serialization
{

namespace meta
{

template <typename>
struct is_std_basic_string : std::false_type {};

template <typename Char, typename Traits, typename Alloc>
struct is_std_basic_string<std::basic_string<Char, Traits, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_WRITE_ARCHIVE_GENERIC(string, meta::is_std_basic_string<T>::value)
{
    const auto string_size = string.size();

    archive & string_size;

    // explicit auto declaration of character without &
    for (auto character : string)
        archive & character;

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(string, meta::is_std_basic_string<T>::value)
{
    using size_type = typename T::size_type;

    size_type string_size;
    archive & string_size;

    string.resize(string_size);

    for(auto& character : string)
        archive & character;

    return archive;
}

} // namespace library

} // namespace serialization

#endif // SERIALIZATION_SUPPORT_STRING_HPP
