#ifndef SF_OARCHIVE_HPP
#define SF_OARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <unordered_map> // unordered_map

#include <SerializationFixture/Core/ArchiveBase.hpp>
#include <SerializationFixture/Core/Memory.hpp>
#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Dynamic/InstantiableRegistry.hpp>

#include <SerializationFixture/StreamWrapper.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
{

template <class StreamWrapperType>
class oarchive_t : public ioarchive_t
{
public:
    template <typename VoidPointerType>
    struct oarchive_track_overload_t
    {
    protected:
        std::unordered_map<std::uintptr_t, bool> xxpointer;

    public:
        auto pointer(VoidPointerType const&) noexcept -> decltype(xxpointer)& { return xxpointer; }
    };

    template <typename... VoidPointerTypes>
    struct oarchive_track_t : public oarchive_track_overload_t<VoidPointerTypes>...
    {
    protected:
        std::unordered_map<std::uintptr_t, std::unordered_map<::xxsf_instantiable_traits_key_type, bool>> xxhierarchy;

    public:
        using oarchive_track_overload_t<VoidPointerTypes>::pointer...;
        auto hierarchy() noexcept -> decltype(xxhierarchy)& { return xxhierarchy; }

    #ifdef SF_DEBUG
        template <typename PointerType>
        bool is_mixed(std::uintptr_t refer_key, PointerType const& pointer) const noexcept
        {
            using pointer_traits = memory::pointer_traits<PointerType>;
            using void_pointer = typename pointer_traits::template pointer_template<void>;

            return ((std::is_same_v<void_pointer, VoidPointerTypes> ?
                    0 : oarchive_track_overload_t<VoidPointerTypes>::xxpointer.count(refer_key)) + ...) > 0;
        }
    #endif // SF_DEBUG
    };

public:
    using TrackingType = oarchive_track_t<INSTANTIABLE_VOID_POINTER_TYPES>;

private:
    StreamWrapperType xxstream;
    TrackingType xxtracking;

public:
    template <typename OutputStreamType>
    oarchive_t(OutputStreamType& stream, ::xxsf_archive_type_key_type type = bin)
        : ioarchive_t(::xxsf_archive_traits<oarchive_t>::key, type, false)
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
          typename OutputStreamType>
oarchive_t<StreamWrapperTemplate<OutputStreamType>> oarchive(OutputStreamType& stream)
{
    return { stream };
}

template <class StreamWrapperType,
          typename OutputStreamType>
oarchive_t<StreamWrapperType> oarchive(OutputStreamType& stream)
{
    return { stream };
}

namespace meta
{

template <class StreamWrapperType>
struct is_oarchive<oarchive_t<StreamWrapperType>> : std::true_type {};

} // namespace meta

} // namespace sf

#endif // SF_OARCHIVE_HPP
