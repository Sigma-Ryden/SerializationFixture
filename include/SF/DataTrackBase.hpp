#ifndef SF_DATA_TRACK_BASE_HPP
#define SF_DATA_TRACK_BASE_HPP

#include <type_traits> // is_same

#include <SF/Core/Memory.hpp>

namespace sf
{

namespace tracking
{

using shared_t = meta::shared_common_t;
using raw_t = meta::raw_common_t;

template <typename T>
struct track_traits;

template <typename T>
struct track_traits<memory::shared_ptr<T>>
{
    using type = tracking::shared_t;
};

template <typename T>
struct track_traits<memory::raw_ptr<T>>
{
    using type = tracking::raw_t;
};

template <typename T>
struct reverse_traits;

template <>
struct reverse_traits<shared_t>
{
    using type = raw_t;
};

template <>
struct reverse_traits<raw_t>
{
    using type = shared_t;
};

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_shared : std::is_same<T, tracking::shared_t> {};
template <typename T> struct is_track_raw : std::is_same<T, tracking::raw_t> {};

} // namespace meta

} // namespace sf

#endif // SF_DATA_TRACK_BASE_HPP
