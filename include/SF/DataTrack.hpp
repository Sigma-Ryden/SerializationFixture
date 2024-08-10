#ifndef SF_DATA_TRACK_HPP
#define SF_DATA_TRACK_HPP

#include <memory> // addressof

#include <SF/Core/SerializatonBase.hpp>

#include <SF/DataTrackBase.hpp>

#include <SF/NativeSaveLoad.hpp>
#include <SF/ApplyFunctor.hpp>
#include <SF/Strict.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace tracking
{

template <typename TrackType, class ArchiveType, typename KeyType,
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
bool is_track(ArchiveType& archive, KeyType key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class ArchiveType, typename KeyType,
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
bool is_mixed(ArchiveType& archive, KeyType key)
{
    using reverse_track_type = typename reverse_traits<TrackType>::type;
    return is_track<reverse_track_type>(archive, key);
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::is_pointer<PointerType>>::value)>
void track(ArchiveType& archive, PointerType& pointer)
{
    using track_type = typename tracking::track_traits<PointerType>::type;

    auto const key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

#ifdef SF_DEBUG
    if (is_mixed<track_type>(archive, key))
        throw "Mixed pointer tracking is not allowed.";
#endif // SF_DEBUG

    auto& is_tracking = archive.template tracking<track_type>()[key];

    if (not is_tracking)
    {
        is_tracking = true;
        strict(archive, pointer); // call the strict serialization of not tracking pointer
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_oarchive<ArchiveType>,
                                meta::negation<meta::is_pointer<SerializableType>>>::value)>
void track(ArchiveType& archive, SerializableType& data)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    auto address = memory::pure(std::addressof(data));
    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.template tracking<tracking::raw_t>()[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::is_pointer<PointerType>>::value)>
void track(ArchiveType& archive, PointerType& pointer)
{
    using track_type = typename tracking::track_traits<PointerType>::type;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    auto const key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

    auto& address = archive.template tracking<track_type>()[key];

    if (address == nullptr)
    {
        // call the strict serialization of not tracking pointer
        strict(archive, pointer, address);
    }
    else
    {
        detail::native_load(archive, pointer, address);
    }
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(meta::all<meta::is_iarchive<ArchiveType>,
                                meta::negation<meta::is_pointer<SerializableType>>>::value)>
void track(ArchiveType& archive, SerializableType& data)
{
    using key_type = typename ArchiveType::TrackingKeyType;

    key_type key{};
    archive & key;

    auto& address = archive.template tracking<tracking::raw_t>()[key];

    if (address != nullptr)
        throw  "The read tracking data is already tracked.";

    address = memory::pure(std::addressof(data));

    archive & data;
}

template <class ArchiveType, typename PointerType,
          SF_REQUIRES(meta::all<meta::is_ioarchive<PointerType>,
                                meta::is_serializable_raw_pointer<PointerType>>::value)>
void raw(ArchiveType& archive, PointerType& pointer)
{
    if (detail::refer_key(archive, pointer)) // serialize refer info
        strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

template <typename SerializableType>
struct track_functor_t : apply_functor_t
{
    SerializableType& data;

    track_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { tracking::track(archive, data); }
};

template <typename SerializableType>
struct raw_functor_t : apply_functor_t
{
    SerializableType& data;

    raw_functor_t(SerializableType& data) noexcept : data(data) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { tracking::raw(archive, data); }
};

} // namespace apply

namespace tracking
{

template <typename SerializableType>
apply::track_functor_t<SerializableType> track(SerializableType& data) noexcept { return { data }; }

template <typename SerializableType>
apply::raw_functor_t<SerializableType> raw(SerializableType& data) noexcept { return { data }; }

} // namespace tracking

} // namespace sf

#endif // SF_DATA_TRACK_HPP
