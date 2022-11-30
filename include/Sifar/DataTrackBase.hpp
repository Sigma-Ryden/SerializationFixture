#ifndef SIFAR_DATA_TRACK_BASE_HPP
#define SIFAR_DATA_TRACK_BASE_HPP

#include <type_traits> // is_same

#include <Sifar/Memory.hpp>

namespace sifar
{

namespace tracking
{

using Shared = meta::shared_type;
using Raw = meta::raw_type;
using Common = meta::common_type;

template <typename T>
struct track_trait;

template <typename T>
struct track_trait<memory::shared_ptr<T>>
{
    using trait = tracking::Shared;
};

template <typename T>
struct track_trait<memory::raw_ptr<T>>
{
    using trait = tracking::Raw;
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

template <typename T> constexpr bool is_track_common() noexcept
{
    return std::is_same<T, tracking::Common>::value;
}

} // namespace meta

} // namespace sifar

#endif // SIFAR_DATA_TRACK_BASE_HPP
