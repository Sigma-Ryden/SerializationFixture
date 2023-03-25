#ifndef SIRAF_DATA_TRACK_BASE_HPP
#define SIRAF_DATA_TRACK_BASE_HPP

#include <type_traits> // is_same

#include <Siraf/Memory/Memory.hpp>

namespace siraf
{

namespace tracking
{

using Shared = meta::shared_type;
using Raw = meta::raw_type;

template <typename T>
struct track_trait;

template <typename T>
struct track_trait<memory::shared_ptr<T>>
{
    using type = tracking::Shared;
};

template <typename T>
struct track_trait<memory::raw_ptr<T>>
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

template <typename T> constexpr bool is_track_shared() noexcept
{
    return std::is_same<T, tracking::Shared>::value;
}

template <typename T> constexpr bool is_track_raw() noexcept
{
    return std::is_same<T, tracking::Raw>::value;
}

} // namespace meta

} // namespace siraf

#endif // SIRAF_DATA_TRACK_BASE_HPP
