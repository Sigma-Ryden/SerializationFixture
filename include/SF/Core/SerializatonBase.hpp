#ifndef SF_SERIALIZATON_BASE_HPP
#define SF_SERIALIZATON_BASE_HPP

#include <type_traits> // is_base_of

namespace sf
{

namespace core
{

struct ArchiveBase;

struct ReadArchiveType {};
struct WriteArchiveType {};

} // namespace core

namespace meta
{

template <class T> constexpr bool is_base_archive() noexcept
{
    return std::is_base_of<core::ArchiveBase, T>::value;
}

template <class T> constexpr bool is_read_archive() noexcept
{
    return std::is_base_of<core::ReadArchiveType, T>::value;
}

template <class T> constexpr bool is_write_archive() noexcept
{
    return std::is_base_of<core::WriteArchiveType, T>::value;
}

template <class T> constexpr bool is_archive() noexcept
{
    return is_read_archive<T>() or is_write_archive<T>();
}

template <class T> constexpr bool is_Save() noexcept { return is_write_archive<T>(); }
template <class T> constexpr bool is_Load() noexcept { return is_read_archive<T>(); }
template <class T> constexpr bool is_SaveLoad() noexcept { return is_archive<T>(); }

} // namespace meta

} // namespace sf

#endif // SF_SERIALIZATON_BASE_HPP
