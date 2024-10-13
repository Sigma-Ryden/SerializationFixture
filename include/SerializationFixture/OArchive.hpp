#ifndef SF_OARCHIVE_HPP
#define SF_OARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <unordered_map> // unordered_map

#include <SerializationFixture/Core/ArchiveBase.hpp>

#include <SerializationFixture/Dynamic/ExternRegistry.hpp>

#include <SerializationFixture/Core/Memory.hpp>

#include <SerializationFixture/DataTrackBase.hpp>
#include <SerializationFixture/HierarchyTrack.hpp>
#include <SerializationFixture/StreamWrapper.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t>
class oarchive_t : public core::ioarchive_t, public core::oarchive_common_t
{
public:
    using TrackingKeyType = std::uintptr_t;

private:
    StreamWrapperType xxarchive;

    std::unordered_map<TrackingKeyType, bool> xxtrack_shared;
    std::unordered_map<TrackingKeyType, bool> xxtrack_raw;

    tracking::hierarchy_track_t<std::uintptr_t> xxtrack_hierarchy;

    RegistryType xxregistry;

public:
    template <typename OutputStreamType>
    oarchive_t(OutputStreamType& stream) : core::ioarchive_t(::xxsf_archive_traits<oarchive_t>::key, false)
        , xxarchive{stream}, xxtrack_shared(), xxtrack_raw(), xxtrack_hierarchy(), xxregistry() {}

    StreamWrapperType& stream() noexcept { return xxarchive; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_shared<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_shared)& { return xxtrack_shared; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_raw<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_raw)& { return xxtrack_raw; }

    template <typename TrackType,
              SF_REQUIRES(meta::is_track_hierarchy<TrackType>::value)>
    auto tracking() noexcept -> decltype(xxtrack_hierarchy)& { return xxtrack_hierarchy; }

    RegistryType& registry() noexcept { return xxregistry; }

    template <typename SerializableType>
    oarchive_t& operator<< (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType>
    oarchive_t& operator& (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType, typename... SerializableTypes>
    oarchive_t& operator() (SerializableType const& data, SerializableTypes const&... datas)
    {
        ::xxsf_save<SerializableType>(*this, const_cast<SerializableType&>(data));
        return operator()(datas...);
    }

    oarchive_t& operator() () noexcept { return *this; }
};

// create default oarchive_t with wrapper::obyte_stream_t<>
template <typename OutputStreamType>
oarchive_t<wrapper::obyte_stream_t<OutputStreamType>> oarchive(OutputStreamType& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapperTemplate,
          class RegistryType = dynamic::extern_registry_t,
          typename OutputStreamType>
oarchive_t<StreamWrapperTemplate<OutputStreamType>, RegistryType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t,
          typename OutputStreamType>
oarchive_t<StreamWrapperType, RegistryType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

} // namespace sf

#endif // SF_OARCHIVE_HPP
