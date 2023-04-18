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
struct track_trait;

template <typename T>
struct track_trait<Memory::shared_ptr<T>>
{
    using type = tracking::Shared;
};

template <typename T>
struct track_trait<Memory::raw_ptr<T>>
{
    using type = tracking::Raw;
};

template <typename T>
struct reverse_trait;

template <>
struct reverse_trait<Shared>
{
    using type = Raw;
};

template <>
struct reverse_trait<Raw>
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
