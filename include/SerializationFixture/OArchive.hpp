#ifndef SF_OARCHIVE_HPP
#define SF_OARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <unordered_map> // unordered_map

#include <SerializationFixture/Core/ArchiveBase.hpp>
#include <SerializationFixture/Core/Memory.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Dynamic/InstantiableTraits.hpp>

#include <SerializationFixture/StreamWrapper.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

class oarchive_track_t
{
private:
    std::unordered_map<std::uintptr_t, bool> xxshared;
    std::unordered_map<std::uintptr_t, bool> xxraw;

    std::unordered_map<std::uintptr_t, std::unordered_map<::xxsf_instantiable_traits_key_type, bool>> xxhierarchy;

public:
    template <typename SerializableType>
    auto pointer(std::shared_ptr<SerializableType> const&) noexcept -> decltype(xxshared)& { return xxshared; }

    template <typename SerializableType>
    auto pointer(SerializableType*) noexcept -> decltype(xxraw)& { return xxraw; }

    auto hierarchy() noexcept -> decltype(xxhierarchy)& { return xxhierarchy; }

#ifdef SF_DEBUG
    template <typename SerializableType>
    bool is_mixed(std::uintptr_t refer_key, std::shared_ptr<SerializableType> const&) const noexcept
    {
        return xxraw.count(refer_key)>0;
    }

    template <typename SerializableType>
    bool is_mixed(std::uintptr_t refer_key, SerializableType*) const noexcept
    {
        return xxshared.count(refer_key)>0;
    }
#endif // SF_DEBUG
};

template <class StreamWrapperType,
          class TrackingType = oarchive_track_t>
class oarchive_t : public ioarchive_t
{
private:
    StreamWrapperType xxstream;
    TrackingType xxtracking;

public:
    template <typename OutputStreamType>
    oarchive_t(OutputStreamType& stream) : ioarchive_t(::xxsf_archive_traits<oarchive_t>::key, false)
        , xxstream{stream}, xxtracking() {}

    StreamWrapperType& stream() noexcept { return xxstream; }
    TrackingType& tracking() noexcept { return xxtracking; }

    template <typename SerializableType>
    oarchive_t& operator<< (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType>
    oarchive_t& operator& (SerializableType const& data) { return operator()(data); }

    template <typename SerializableType, typename... SerializableTypes>
    oarchive_t& operator() (SerializableType const& data, SerializableTypes const&... datas)
    {
        ::xxsf<SerializableType>::save(*this, const_cast<SerializableType&>(data));
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
          class TrackingType = oarchive_track_t,
          typename OutputStreamType>
oarchive_t<StreamWrapperTemplate<OutputStreamType>, TrackingType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          class TrackingType = oarchive_track_t,
          typename OutputStreamType>
oarchive_t<StreamWrapperType, TrackingType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

namespace meta
{

template <class StreamWrapperType, class TrackingType>
struct is_oarchive<oarchive_t<StreamWrapperType, TrackingType>> : std::true_type {};

} // namespace meta

} // namespace sf

#endif // SF_OARCHIVE_HPP
