#ifndef SF_HIERARCHY_TRACK_HPP
#define SF_HIERARCHY_TRACK_HPP

#include <utility> // pair
#include <unordered_map> // unordered_map

#include <SF/Core/Hash.hpp>

#include <SF/Dynamic/InstantiableTraits.hpp>

#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace detail
{

template <typename HashType = let::u64>
struct pair_hash_t
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

struct hierarchy_t {};

template <typename KeyType, typename TraitsType = ::xxsf_instantiable_traits_key_type>
using hierarchy_track_t = std::unordered_map<std::pair<KeyType, TraitsType>, bool, detail::pair_hash_t<TraitsType>>;

} // namespace tracking

namespace meta
{

template <typename TrackType> struct is_track_hierarchy : std::is_same<TrackType, tracking::hierarchy_t> {};

} // namespace meta

} // namespace sf

#endif // SF_HIERARCHY_TRACK_HPP
