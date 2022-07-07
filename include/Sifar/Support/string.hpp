#ifndef SIFAR_SUPPORT_STRING_HPP
#define SIFAR_SUPPORT_STRING_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Utility.hpp>

namespace sifar
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

SERIALIZATION_SAVE_DATA(string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size = string.size();

    archive & size;

    archive.stream().write
    (
        string.data(),
        string.size() * sizeof(char_type)
    );

    return archive;
}

SERIALIZATION_LOAD_DATA(string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size;
    archive & size;

    string.resize(size);

    archive.stream().read
    (
        const_cast<char_type*>(string.data()),
        string.size() * sizeof(char_type)
    );

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_basic_string<T>::value)

#endif // SIFAR_SUPPORT_STRING_HPP
