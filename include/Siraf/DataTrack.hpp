#ifndef SIRAF_DATA_TRACK_HPP
#define SIRAF_DATA_TRACK_HPP

#include <memory> // addressof

#include <Siraf/DataTrackBase.hpp>

#include <Siraf/NativeSaveLoad.hpp>
#include <Siraf/ApplyFunctor.hpp>
#include <Siraf/Strict.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace tracking
{

template <typename TrackType, class Archive, typename key_type,
          SIREQUIRE(meta::is_archive<Archive>())>
bool is_track(Archive& archive, key_type key)
{
    auto& item = archive.template tracking<TrackType>();
    return item.find(key) != item.end();
}

template <typename TrackType, class Archive, typename key_type,
          SIREQUIRE(meta::is_archive<Archive>())>
bool is_mixed(Archive& archive, key_type key)
{
    using reverse_track_type = typename reverse_trait<TrackType>::type;

    return is_track<reverse_track_type>(archive, key);
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and meta::is_pointer<T>())>
void track(WriteArchive& archive, T& pointer)
{
    using key_type   = typename WriteArchive::TrackingKeyType;
    using track_type = typename tracking::track_trait<T>::type;

#ifndef SIRAF_NULLPTR_DISABLE
    const auto success = detail::is_refer(archive, pointer); // serialize refer info
    if (not success) return;
#endif // SIRAF_NULLPTR_DISABLE

    auto pure = Memory::pure(pointer);
    auto key = reinterpret_cast<key_type>(Memory::raw(pure));

#ifdef SIRAF_DEBUG
    if (is_mixed<track_type>(archive, key))
        throw "Mixed pointer tracking is not allowed.";
#endif // SIRAF_DEBUG

    auto& is_tracking = archive.template tracking<track_type>()[key];

    if (not is_tracking)
    {
        is_tracking = true;

        archive & key;
        strict(archive, pointer); // call the strict serialization of not tracking pointer
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and not meta::is_pointer<T>())>
void track(WriteArchive& archive, T& data)
{
    using key_type = typename WriteArchive::TrackingKeyType;

    auto address = Memory::pure(std::addressof(data));
    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.template tracking<tracking::Raw>()[key];

    if (is_tracking)
        throw "The write tracking data is already tracked.";

    is_tracking = true;

    archive & key;
    archive & data;
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and meta::is_pointer<T>())>
void track(ReadArchive& archive, T& pointer)
{
    using key_type   = typename ReadArchive::TrackingKeyType;
    using track_type = typename tracking::track_trait<T>::type;

    if (pointer != nullptr)
        throw "The read track pointer must be initialized to nullptr.";

#ifndef SIRAF_NULLPTR_DISABLE
    const auto success = detail::is_refer(archive, pointer); // serialize refer info
    if (not success) return;
#endif // SIRAF_NULLPTR_DISABLE

    key_type key;
    archive & key;

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

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and not meta::is_pointer<T>())>
void track(ReadArchive& archive, T& data)
{
    using key_type = typename ReadArchive::TrackingKeyType;

    key_type key;
    archive & key;

    auto& item = archive.template tracking<tracking::Raw>()[key];

    if (item.address != nullptr)
        throw  "The read tracking data is already tracked.";

    item.address = Memory::pure(std::addressof(data));

    archive & data;
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<T>() and
                    meta::is_serializable_raw_pointer<T>())>
void raw(Archive& archive, T& pointer)
{
#ifndef SIRAF_NULLPTR_DISABLE
    if (detail::is_refer(archive, pointer))
#endif //SIRAF_NULLPTR_DISABLE
    strict(archive, pointer);
}

} // namespace tracking

namespace apply
{

template <typename T>
struct TrackFunctor : ApplyFunctor
{
    T& data;

    template <class Archive>
    void operator() (Archive& archive) { tracking::track(archive, data); }
};

template <typename T>
struct RawFunctor : ApplyFunctor
{
    T& data;

    template <class Archive>
    void operator() (Archive& archive) { tracking::raw(archive, data); }
};

} // namespace apply

namespace tracking
{

template <typename T> apply::TrackFunctor<T> track(T& data) { return { {}, data }; }
template <typename T> apply::RawFunctor<T> raw(T& data) { return { {}, data }; }

} // namespace tracking

} // namespace siraf

#endif // SIRAF_DATA_TRACK_HPP
