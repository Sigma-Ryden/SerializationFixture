#ifndef SF_SERIALIZATON_BASE_HPP
#define SF_SERIALIZATON_BASE_HPP

#include <SF/Detail/Meta.hpp>

namespace sf
{

namespace core
{

struct ioarchive_t;

struct iarchive_common_t {};
struct oarchive_common_t {};

} // namespace core

namespace meta
{

template <class T> struct is_iarchive : std::is_base_of<core::iarchive_common_t, T> {};
template <class T> struct is_oarchive : std::is_base_of<core::oarchive_common_t, T> {};

template <class T> struct is_ioarchive : one<is_iarchive<T>, is_oarchive<T>> {};

} // namespace meta

} // namespace sf

#endif // SF_SERIALIZATON_BASE_HPP
