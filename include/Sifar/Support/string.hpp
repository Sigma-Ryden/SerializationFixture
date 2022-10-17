#ifndef SIFAR_SUPPORT_STRING_HPP
#define SIFAR_SUPPORT_STRING_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/Compress.hpp>

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

inline namespace library
{

SERIALIZATION_SAVE_DATA(string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size = string.size();
    archive & size;

    compress::zip(archive, string);

    return archive;
}

SERIALIZATION_LOAD_DATA(string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size;
    archive & size;

    string.resize(size);
    compress::zip(archive, string);

    return archive;
}

} // inline namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_basic_string<T>::value)

#endif // SIFAR_SUPPORT_STRING_HPP
