#ifndef SF_HIERARCHY_TRACK_BASE_HPP
#define SF_HIERARCHY_TRACK_BASE_HPP

#include <type_traits> // is_same

namespace sf
{

namespace tracking
{

struct hierarchy_t {};

} // namespace tracking

namespace meta
{

template <typename TrackType> struct is_track_hierarchy : std::is_same<TrackType, tracking::hierarchy_t> {};

} // namespace meta

} // namespace sf

#endif // SF_HIERARCHY_TRACK_BASE_HPP
