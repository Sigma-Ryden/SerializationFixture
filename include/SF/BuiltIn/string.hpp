#ifndef SF_BUILT_IN_STRING_HPP
#define SF_BUILT_IN_STRING_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/ExternSerialization.hpp>
#include <SF/Compress.hpp>

namespace sf
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

EXTERN_CONDITIONAL_SERIALIZATION(Save, string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size = string.size();
    archive & size;

    compress::zip(archive, string);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, string, meta::is_std_basic_string<T>::value)
{
    using char_type = typename T::value_type;

    let::u64 size{};
    archive & size;

    string.resize(size);
    compress::zip(archive, string);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_basic_string<T>::value)

#endif // SF_BUILT_IN_STRING_HPP
