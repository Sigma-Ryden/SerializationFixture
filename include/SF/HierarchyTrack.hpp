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

template <typename HashType = let::u64>
struct PairHash
{
    template <typename T1, typename T2>
    HashType operator() (const std::pair<T1, T2>& pair) const noexcept
    {
        HashType seed{};

        detail::hash_combine(seed, pair.first);
        detail::hash_combine(seed, pair.second);

        return seed;
    }
};

} // namespace detail

namespace tracking
{

struct Hierarchy {};

template <typename KeyType, typename TraitType = core::InstantiableTraitBase::key_type>
using HierarchyTrack = std::unordered_map<std::pair<KeyType, TraitType>, bool, detail::PairHash<TraitType>>;

} // namespace tracking

namespace meta
{

template <typename T> struct is_track_hierarchy : std::is_same<T, tracking::Hierarchy> {};

} // namespace meta

} // namespace sf

#endif // SF_HIERARCHY_TRACK_HPP
