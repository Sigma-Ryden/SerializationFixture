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

template <typename TrackType, class Archive, typename KeyType,
          SFREQUIRE(meta::is_archive<Archive>::value)>
bool is_track(Archive& archive, KeyType key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class Archive, typename KeyType,
          SFREQUIRE(meta::is_archive<Archive>::value)>
bool is_mixed(Archive& archive, KeyType key)
{
    using reverse_track_type = typename reverse_trait<TrackType>::type;
    return is_track<reverse_track_type>(archive, key);
}

template <class OArchive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<OArchive>,
                              meta::is_pointer<T>>::value)>
void track(OArchive& archive, T& pointer)
{
    using track_type = typename tracking::track_trait<T>::type;

    auto key = detail::refer_key(archive, pointer); // serialize refer info
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

template <class OArchive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<OArchive>,
                              meta::negation<meta::is_pointer<T>>>::value)>
void track(OArchive& archive, T& data)
{
    using key_type = typename OArchive::TrackingKeyType;

    auto address = Memory::pure(std::addressof(data));
    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.template tracking<tracking::Raw>()[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class IArchive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<IArchive>,
                              meta::is_pointer<T>>::value)>
void track(IArchive& archive, T& pointer)
{
    using track_type = typename tracking::track_trait<T>::type;

#ifndef SF_GARBAGE_CHECK_DISABLE
    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";
#endif // SF_GARBAGE_CHECK_DISABLE

    auto key = detail::refer_key(archive, pointer); // serialize refer info
    if (not key) return;

    auto& item = archive.template tracking<track_type>()[key];

    if (item.address == nullptr)
    {
        // call the strict serialization of not tracking pointer
        strict(archive, pointer, item.address);
    }
    else
    {
        detail::native_load(archive, pointer, item.address);
    }
}

template <class IArchive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<IArchive>,
                              meta::negation<meta::is_pointer<T>>>::value)>
void track(IArchive& archive, T& data)
{
    using key_type = typename IArchive::TrackingKeyType;

    key_type key{};
    archive & key;

    auto& item = archive.template tracking<tracking::Raw>()[key];

    if (item.address != nullptr)
        throw  "The read tracking data is already tracked.";

    item.address = Memory::pure(std::addressof(data));

    archive & data;
}

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_archive<T>,
                              meta::is_serializable_raw_pointer<T>>::value)>
void raw(Archive& archive, T& pointer)
{
    if (detail::refer_key(archive, pointer)) // serialize refer info
        strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

template <typename T>
struct TrackFunctor : ApplyFunctor
{
    T& data;

    TrackFunctor(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { tracking::track(archive, data); }
};

template <typename T>
struct RawFunctor : ApplyFunctor
{
    T& data;

    RawFunctor(T& data) noexcept : data(data) {}

    template <class Archive>
    void operator() (Archive& archive) const { tracking::raw(archive, data); }
};

} // namespace apply

namespace tracking
{

template <typename T> apply::TrackFunctor<T> track(T& data) noexcept { return { data }; }
template <typename T> apply::RawFunctor<T> raw(T& data) noexcept { return { data }; }

} // namespace tracking

} // namespace sf

#endif // SF_DATA_TRACK_HPP
