#ifndef SIRAF_WRITE_ARCHIVE_HPP
#define SIRAF_WRITE_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map

#include <Siraf/ArchiveBase.hpp>
#include <Siraf/Dynamic/Registry.hpp>

#include <Siraf/Memory/Memory.hpp>
#include <Siraf/DataTrackBase.hpp>

#include <Siraf/Detail/StaticMessage.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace wrapper
{

template <typename OutStream>
class OutStreamWrapper
{
private:
    OutStream& stream_;

public:
    OutStreamWrapper(OutStream& stream) : stream_(stream) {}

    template <typename T>
    void call(const T* data, std::size_t memory_size)
    {
        stream_.write(memory::const_byte_cast(data), memory_size);
    }

    OutStream& get() noexcept { return stream_; }
};

} // namespace wrapper

template <class OutStream,
          class StreamWrapper = wrapper::OutStreamWrapper<OutStream>,
          class Registry = dynamic::ExternRegistry>
class WriteArchive : public core::ArchiveBase
{
    SERIALIZATION_ARCHIVE(WriteArchive)

public:
    using TrackingKeyType = std::uintptr_t;
    using TrackingTable   = std::unordered_map<TrackingKeyType, bool>;

private:
    StreamWrapper archive_;
    TrackingTable track_common_;
    Registry registry_;

public:
    WriteArchive(OutStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType = tracking::Common,
              SIREQUIRE(meta::is_track_common<TrackType>())>
    auto tracking() noexcept -> TrackingTable& { return track_common_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator<< (T&& data) -> WriteArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> WriteArchive&;

    auto operator() () noexcept -> WriteArchive& { return *this; }
};

template <class OutStream>
WriteArchive<OutStream> oarchive(OutStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class OutStream>
WriteArchive<OutStream, StreamWrapper<OutStream>, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class OutStream>
WriteArchive<OutStream, StreamWrapper, Registry> oarchive(OutStream& stream)
{
    return { stream };
}

template <class OutStream, class StreamWrapper, class Registry>
WriteArchive<OutStream, StreamWrapper, Registry>::WriteArchive(OutStream& stream)
    : archive_{stream}, track_common_(), registry_()
{
}

template <class OutStream, class StreamWrapper, class Registry>
template <typename T>
auto WriteArchive<OutStream, StreamWrapper, Registry>::operator<< (
    T&& data) -> WriteArchive&
{
    return (*this) & data;
}

template <class OutStream, class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto WriteArchive<OutStream, StreamWrapper, Registry>::operator() (
    T& data, Tn&... data_n) -> WriteArchive&
{
    (*this) & data;
    return (*this)(data_n...);
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and meta::is_unsupported<T>())>
WriteArchive& operator& (WriteArchive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'siraf::WriteArchive'.");

    return archive;
}

template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and not meta::is_registered<T>())>
WriteArchive& operator& (WriteArchive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        STATIC_MESSAGE_UNREGISTERED_TYPE(siraf::WriteArchive, Save));

    return archive;
}

} // namespace siraf

#endif // SIRAF_WRITE_ARCHIVE_HPP
