#ifndef SF_DATA_TRACK_BASE_HPP
#define SF_DATA_TRACK_BASE_HPP

#include <type_traits> // is_same

#include <SerializationFixture/Core/Memory.hpp>

namespace sf
{

namespace tracking
{

using shared_t = meta::shared_common_t;
using raw_t = meta::raw_common_t;

template <typename PointerType>
struct track_traits;

template <typename Type>
struct track_traits<memory::shared_ptr<Type>>
{
    using type = tracking::shared_t;
};

template <typename Type>
struct track_traits<memory::raw_ptr<Type>>
{
    using type = tracking::raw_t;
};

template <typename TrackType>
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

template <typename TrackType> struct is_track_shared : std::is_same<TrackType, tracking::shared_t> {};
template <typename TrackType> struct is_track_raw : std::is_same<TrackType, tracking::raw_t> {};

} // namespace meta

} // namespace sf

#endif // SF_DATA_TRACK_BASE_HPP
