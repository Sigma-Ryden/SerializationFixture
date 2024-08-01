#ifndef SF_OARCHIVE_HPP
#define SF_OARCHIVE_HPP

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
class oarchive_t : public core::ioarchive_t, public core::oarchive_common_t
{
public:
    using TrackingKeyType = std::uintptr_t;
    using TrackingTable = std::unordered_map<TrackingKeyType, bool>;
    using HierarchyTrackingTable = tracking::hierarchy_track_t<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable track_shared_;
    TrackingTable track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    template <typename OutStream> oarchive_t(OutStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_shared_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> TrackingTable& { return track_raw_; }

    template <typename TrackType,
              SF_REQUIRE(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator<< (T&& data) -> oarchive_t&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> oarchive_t&;

    auto operator() () noexcept -> oarchive_t& { return *this; }
};

// create default oarchive_t with wrapper::obyte_stream_t<>
template <typename OutStream>
oarchive_t<wrapper::obyte_stream_t<OutStream>> oarchive(OutStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename OutStream>
oarchive_t<StreamWrapper<OutStream>, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::extern_registry_t,
          typename OutStream>
oarchive_t<StreamWrapper, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper, class Registry>
template <typename OutStream>
oarchive_t<StreamWrapper, Registry>::oarchive_t(OutStream& stream)
    : core::ioarchive_t(core::archive_traits_key_t<oarchive_t>::key, false)
    , archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class StreamWrapper, class Registry>
template <typename T>
auto oarchive_t<StreamWrapper, Registry>::operator<< (T&& data) -> oarchive_t&
{
    return (*this) & std::forward<T>(data);
}

template <class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto oarchive_t<StreamWrapper, Registry>::operator() (T& data, Tn&... data_n) -> oarchive_t&
{
    (*this) & data;
    return operator()(data_n...);
}

} // namespace sf

#endif // SF_OARCHIVE_HPP
