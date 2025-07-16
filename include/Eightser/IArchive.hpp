#ifndef EIGHTSER_IARCHIVE_HPP
#define EIGHTSER_IARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <unordered_map> // unordered_map

#include <Eightser/Core/ArchiveBase.hpp>
#include <Eightser/Core/Memory.hpp>
#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Dynamic/InstantiableRegistry.hpp>

#include <Eightser/StreamWrapper.hpp>

#include <Eightser/Detail/Meta.hpp>
#include <Eightser/Detail/MetaMacro.hpp>

namespace eightser
{

template <class StreamWrapperType>
class iarchive_t : public ioarchive_t
{
public:
    template <typename VoidPointerType>
    struct iarchive_track_overload_t
    {
    protected:
        std::unordered_map<std::uintptr_t, VoidPointerType> xxpointer;

    public:
        auto pointer(VoidPointerType const&) noexcept -> decltype(xxpointer)& { return xxpointer; }
    };

    template <typename... VoidPointerTypes>
    struct iarchive_track_t : iarchive_track_overload_t<VoidPointerTypes>...
    {
    protected:
        std::unordered_map<std::uintptr_t, std::unordered_map<::xxeightser_instantiable_traits_key_type, bool>> xxhierarchy;

    public:
        using iarchive_track_overload_t<VoidPointerTypes>::pointer...;
        auto hierarchy() noexcept -> decltype(xxhierarchy)& { return xxhierarchy; }
    };

public:
    using TrackingType = iarchive_track_t<INSTANTIABLE_VOID_POINTER_TYPES>;

private:
    StreamWrapperType xxstream;
    TrackingType xxtracking;

public:
    template <typename InputStreamType>
    iarchive_t(InputStreamType& stream, ::xxeightser_archive_type_key_type type = bin)
        : ioarchive_t(::xxeightser_archive_traits<iarchive_t>::key, type, true)
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
        ::xxeightser<SerializableType>::load(*this, const_cast<SerializableType&>(data));
        return operator()(datas...);
    }

    iarchive_t& operator() () { return *this; }
};

// create default iarchive_t with wrapper::ibyte_stream_t<>
template <typename InputStreamType>
iarchive_t<wrapper::ibyte_stream_t<InputStreamType>> iarchive(InputStreamType& stream, ::xxeightser_archive_type_key_type type = bin)
{
    return { stream, type };
}

template <template <class, typename...> class StreamWrapperTemplate,
          typename InputStreamType>
iarchive_t<StreamWrapperTemplate<InputStreamType>> iarchive(InputStreamType& stream, ::xxeightser_archive_type_key_type type = bin)
{
    return { stream, type };
}

template <class StreamWrapperType,
          typename InputStreamType>
iarchive_t<StreamWrapperType> iarchive(InputStreamType& stream, ::xxeightser_archive_type_key_type type = bin)
{
    return { stream, type };
}

namespace meta
{

template <class StreamWrapperType>
struct is_iarchive<iarchive_t<StreamWrapperType>> : std::true_type {};

} // namespace meta

} // namespace eightser

#endif // EIGHTSER_IARCHIVE_HPP
