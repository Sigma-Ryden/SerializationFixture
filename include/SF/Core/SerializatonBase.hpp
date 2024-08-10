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

template <class ArchiveType> struct is_iarchive : std::is_base_of<core::iarchive_common_t, ArchiveType> {};
template <class ArchiveType> struct is_oarchive : std::is_base_of<core::oarchive_common_t, ArchiveType> {};

template <class ArchiveType> struct is_ioarchive : one<is_iarchive<ArchiveType>, is_oarchive<ArchiveType>> {};

} // namespace meta

} // namespace sf

#endif // SF_SERIALIZATON_BASE_HPP
