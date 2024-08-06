#ifndef SF_IARCHIVE_HPP
#define SF_IARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <unordered_map> // unordered_map
#include <utility> // forward

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>

#include <SF/Dynamic/ExternRegistry.hpp>

#include <SF/Core/Memory.hpp>

#include <SF/DataTrackBase.hpp>
#include <SF/HierarchyTrack.hpp>
#include <SF/StreamWrapper.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t>
class iarchive_t : public core::ioarchive_t, public core::iarchive_common_t
{
private:
    template <typename VoidPointer>
    struct track_data_t { VoidPointer address = nullptr; };

public:
    using shared_t = track_data_t<memory::shared_ptr<void>>;
    using raw_t = track_data_t<memory::raw_ptr<void>>;

public:
    using TrackingKeyType = std::uintptr_t;

    template <typename TrackData>
    using TrackingTable = std::unordered_map<TrackingKeyType, TrackData>;

    using HierarchyTrackingTable = tracking::hierarchy_track_t<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable<shared_t> track_shared_;
    TrackingTable<raw_t> track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    template <typename InStream> iarchive_t(InStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable<shared_t>& { return track_shared_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable<raw_t>& { return track_raw_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator>> (T const& data) -> iarchive_t&;

    template <typename T>
    auto operator& (T const& data) -> iarchive_t&;

    template <typename T, typename... Tn>
    auto operator() (T const& data, Tn const&... data_n) -> iarchive_t&;

    auto operator() () -> iarchive_t& { return *this; }
};

// create default iarchive_t with wrapper::ibyte_stream_t<>
template <typename InStream>
iarchive_t<wrapper::ibyte_stream_t<InStream>> iarchive(InStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename InStream>
iarchive_t<StreamWrapper<InStream>, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename InStream>
iarchive_t<StreamWrapper, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper, class Registry>
template <typename InStream>
iarchive_t<StreamWrapper, Registry>::iarchive_t(InStream& stream)
    : core::ioarchive_t(core::archive_traits_key_t<iarchive_t>::key, true)
    , archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class StreamWrapper, class Registry>
template <typename T>
auto iarchive_t<StreamWrapper, Registry>::operator>> (T const& data) -> iarchive_t&
{
    return operator()(data);
}

template <class StreamWrapper, class Registry>
template <typename T>
auto iarchive_t<StreamWrapper, Registry>::operator& (T const& data) -> iarchive_t&
{
    return operator()(data);
}

template <class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto iarchive_t<StreamWrapper, Registry>::operator() (T const& data, Tn const&... data_n) -> iarchive_t&
{
    ::xxsf_load<T>(*this, const_cast<T&>(data));
    return operator()(data_n...);
}

} // namespace sf

#endif // SF_IARCHIVE_HPP
