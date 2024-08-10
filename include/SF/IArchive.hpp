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

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t>
class iarchive_t : public core::ioarchive_t, public core::iarchive_common_t
{
public:
    using TrackingKeyType = std::uintptr_t;

private:
    StreamWrapperType xxarchive;

    std::unordered_map<TrackingKeyType, memory::shared_ptr<void>> xxtrack_shared;
    std::unordered_map<TrackingKeyType, memory::raw_ptr<void>> xxtrack_raw;

    tracking::hierarchy_track_t<TrackingKeyType> xxtrack_hierarchy;

    RegistryType xxregistry;

public:
    template <typename InputStreamType>
    iarchive_t(InputStreamType& stream) : core::ioarchive_t(::xxsf_archive_traits<iarchive_t>::key, true)
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
    iarchive_t& operator>> (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType>
    iarchive_t& operator& (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType, typename... SerializableTypes>
    iarchive_t& operator() (SerializableType const& data, SerializableTypes const&... datas)
    {
        ::xxsf_load<SerializableType>(*this, const_cast<SerializableType&>(data));
        return operator()(datas...);
    }

    iarchive_t& operator() () { return *this; }
};

// create default iarchive_t with wrapper::ibyte_stream_t<>
template <typename InputStreamType>
iarchive_t<wrapper::ibyte_stream_t<InputStreamType>> iarchive(InputStreamType& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapperTemplate,
          class RegistryType = dynamic::extern_registry_t,
          typename InputStreamType>
iarchive_t<StreamWrapperTemplate<InputStreamType>, RegistryType> iarchive(InputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          class RegistryType = dynamic::extern_registry_t,
          typename InputStreamType>
iarchive_t<StreamWrapperType, RegistryType> iarchive(InputStreamType& stream)
{
    return { stream };
}

} // namespace sf

#endif // SF_IARCHIVE_HPP
