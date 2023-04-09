#ifndef SF_HIERARCHY_TRACK_HPP
#define SF_HIERARCHY_TRACK_HPP

#include <utility> // pair
#include <unordered_map> // unordered_map

#include <SF/Core/InstantiableTrait.hpp>
#include <SF/Core/Hash.hpp>

#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace detail
{

struct PairHash
{
    template <typename T1, typename T2>
    let::u64 operator() (const std::pair<T1, T2>& pair) const noexcept
    {
        let::u64 seed = 0;

        hash_combine(seed, pair.first);
        hash_combine(seed, pair.second);

        return seed;
    }
};

} // namespace detail

namespace tracking
{

struct Hierarchy {};

template <typename KeyType, typename TraitType = core::InstantiableTraitBase::key_type>
using HierarchyTrack = std::unordered_map<std::pair<KeyType, TraitType>, bool, detail::PairHash>;

} // namespace tracking

namespace meta
{

template <typename T> constexpr bool is_track_hierarchy() noexcept
{
    return std::is_same<T, tracking::Hierarchy>::value;
}

} // namespace meta

} // namespace sf

#endif // SF_HIERARCHY_TRACK_HPP