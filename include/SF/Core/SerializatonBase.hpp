#ifndef SF_SERIALIZATON_BASE_HPP
#define SF_SERIALIZATON_BASE_HPP

#include <SF/Detail/Meta.hpp>

namespace sf
{

namespace core
{

class IOArchive;

struct IArchiveType {};
struct OArchiveType {};

} // namespace core

namespace meta
{

template <class T> struct is_iarchive : std::is_base_of<core::IArchiveType, T> {};
template <class T> struct is_oarchive : std::is_base_of<core::OArchiveType, T> {};

template <class T> struct is_ioarchive : one<is_iarchive<T>, is_oarchive<T>> {};

template <class T> struct is_archive : std::is_base_of<core::IOArchive, T> {};

} // namespace meta

} // namespace sf

#endif // SF_SERIALIZATON_BASE_HPP
