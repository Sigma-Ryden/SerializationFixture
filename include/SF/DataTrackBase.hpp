#ifndef SF_DATA_TRACK_BASE_HPP
#define SF_DATA_TRACK_BASE_HPP

#include <type_traits> // is_same

#include <SF/Core/Memory.hpp>

namespace sf
{

namespace tracking
{

using Shared = meta::shared_type;
using Raw = meta::raw_type;

template <typename T>
struct track_traits;

template <typename T>
struct track_traits<memory_t::shared_ptr<T>>
{
    using type = tracking::Shared;
};

template <typename T>
struct track_traits<memory_t::raw_ptr<T>>
{
    using type = tracking::Raw;
};

template <typename T>
struct reverse_traits;

template <>
struct reverse_traits<Shared>
{
    using type = Raw;
};

template <>
struct reverse_traits<Raw>
{
    using type = Shared;
};

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_shared : std::is_same<T, tracking::Shared> {};
template <typename T> struct is_track_raw : std::is_same<T, tracking::Raw> {};

} // namespace meta

} // namespace sf

#endif // SF_DATA_TRACK_BASE_HPP
