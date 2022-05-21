// This file include all extern type detectors
// You MUST NOT include this file into the #include list below
#ifndef SERIALIZATION_TYPE_DETECTION_HPP
#define SERIALIZATION_TYPE_DETECTION_HPP

#include <type_traits> // true_type, false_type

#include <string> // basic_string

namespace serialization
{

namespace meta
{

namespace detail
{

template <typename>
struct is_std_basic_string : std::false_type {};

template <typename Char, typename Traits, typename Alloc>
struct is_std_basic_string<std::basic_string<Char, Traits, Alloc>> : std::true_type {};

} // namespace detail

template <class T> constexpr bool is_std_basic_string() noexcept
{
    return detail::is_std_basic_string<T>::value;
}

} // namespace meta

} // namespace serialization

#endif // SERIALIZATION_TYPE_DETECTION_HPP
