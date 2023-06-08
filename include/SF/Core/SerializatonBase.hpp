#ifndef SF_SERIALIZATON_BASE_HPP
#define SF_SERIALIZATON_BASE_HPP

#include <SF/Detail/Meta.hpp>

namespace sf
{

namespace core
{

class ArchiveBase;

struct ReadArchiveType {};
struct WriteArchiveType {};

} // namespace core

namespace meta
{

template <class T> struct is_base_archive : std::is_base_of<core::ArchiveBase, T> {};

template <class T> struct is_read_archive : std::is_base_of<core::ReadArchiveType, T> {};
template <class T> struct is_write_archive : std::is_base_of<core::WriteArchiveType, T> {};

template <class T> struct is_archive : one<is_read_archive<T>, is_write_archive<T>> {};

template <class T> struct is_Save : is_write_archive<T> {};
template <class T> struct is_Load : is_read_archive<T> {};

template <class T> struct is_SaveLoad : is_archive<T> {};

} // namespace meta

} // namespace sf

#endif // SF_SERIALIZATON_BASE_HPP
