#ifndef SF_IARCHIVE_HPP
#define SF_IARCHIVE_HPP

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

class iarchive_track_t
{
private:
    std::unordered_map<std::uintptr_t, std::shared_ptr<void>> xxshared;
    std::unordered_map<std::uintptr_t, void*> xxraw;

    std::unordered_map<std::uintptr_t, std::unordered_map<::xxsf_instantiable_traits_key_type, bool>> xxhierarchy;

public:
    template <typename PointerType, SF_REQUIRES(meta::is_std_shared_ptr<PointerType>::value)>
    auto pointer() noexcept -> decltype(xxshared)& { return xxshared; }

    template <typename PointerType, SF_REQUIRES(meta::is_raw_pointer<PointerType>::value)>
    auto pointer() noexcept -> decltype(xxraw)& { return xxraw; }

    auto hierarchy() noexcept -> decltype(xxhierarchy)& { return xxhierarchy; }
};

template <class StreamWrapperType,
          class TrackingType = iarchive_track_t>
class iarchive_t : public ioarchive_t
{
private:
    StreamWrapperType xxstream;
    TrackingType xxtracking;

public:
    template <typename InputStreamType>
    iarchive_t(InputStreamType& stream) : ioarchive_t(::xxsf_archive_traits<iarchive_t>::key, true)
        , xxstream{stream}, xxtracking() {}

    StreamWrapperType& stream() noexcept { return xxstream; }
    TrackingType& tracking() noexcept { return xxtracking; }

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
          class TrackingType = iarchive_track_t,
          typename InputStreamType>
iarchive_t<StreamWrapperTemplate<InputStreamType>> iarchive(InputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          class TrackingType = iarchive_track_t,
          typename InputStreamType>
iarchive_t<StreamWrapperType> iarchive(InputStreamType& stream)
{
    return { stream };
}

namespace meta
{

template <class StreamWrapperType, class TrackingType>
struct is_iarchive<iarchive_t<StreamWrapperType, TrackingType>> : std::true_type {};

} // namespace meta

} // namespace sf

#endif // SF_IARCHIVE_HPP
