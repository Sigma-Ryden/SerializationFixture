#ifndef SIRAF_READ_ARCHIVE_HPP
#define SIRAF_READ_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map
#include <utility> // forward

#include <Siraf/Core/ArchiveBase.hpp>
#include <Siraf/Core/TypeRegistry.hpp>

#include <Siraf/Dynamic/ExternRegistry.hpp>

#include <Siraf/Core/Memory.hpp>

#include <Siraf/DataTrackBase.hpp>
#include <Siraf/HierarchyTrack.hpp>
#include <Siraf/StreamWrapper.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

template <class InStream,
          class StreamWrapper,
          class Registry = dynamic::ExternRegistry>
class ReadArchive : public core::ArchiveBase
{
    SERIALIZATION_ARCHIVE(ReadArchive)

private:
    template <typename VoidPointer>
    struct TrackData { VoidPointer address = nullptr; };

public:
    using Shared = TrackData<Memory::shared_ptr<void>>;
    using Raw = TrackData<Memory::raw_ptr<void>>;

public:
    using TrackingKeyType = std::uintptr_t;

    template <typename TrackData>
    using TrackingTable = std::unordered_map<TrackingKeyType, TrackData>;

    using HierarchyTrackingTable = tracking::HierarchyTrack<TrackingKeyType>;

private:
    StreamWrapper archive_;

    TrackingTable<Shared> track_shared_;
    TrackingTable<Raw> track_raw_;

    HierarchyTrackingTable track_hierarchy_;

    Registry registry_;

public:
    ReadArchive(InStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }

    template <typename TrackType,
              SIREQUIRE(meta::is_track_shared<TrackType>())>
    auto tracking() noexcept -> TrackingTable<Shared>& { return track_shared_; }

    template <typename TrackType,
              SIREQUIRE(meta::is_track_raw<TrackType>())>
    auto tracking() noexcept -> TrackingTable<Raw>& { return track_raw_; }

    template <typename TrackType,
              SIREQUIRE(meta::is_track_hierarchy<TrackType>())>
    auto tracking() noexcept -> HierarchyTrackingTable& { return track_hierarchy_; }

    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T>
    auto operator>> (T&& data) -> ReadArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> ReadArchive&;

    auto operator() () -> ReadArchive& { return *this; }
};

// create default ReadArchive<> with wrapper::IByteStream<>
template <class InStream>
ReadArchive<InStream, wrapper::IByteStream<InStream>> iarchive(InStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class InStream>
ReadArchive<InStream, StreamWrapper<InStream>, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class InStream>
ReadArchive<InStream, StreamWrapper, Registry> iarchive(InStream& stream)
{
    return { stream };
}

template <class InStream, class StreamWrapper, class Registry>
ReadArchive<InStream, StreamWrapper, Registry>::ReadArchive(InStream& stream)
    : archive_{stream}, track_shared_(), track_raw_(), track_hierarchy_(), registry_()
{
}

template <class InStream, class StreamWrapper, class Registry>
template <typename T>
auto ReadArchive<InStream, StreamWrapper, Registry>::operator>> (
    T&& data) -> ReadArchive&
{
    return (*this) & std::forward<T>(data);
}

template <class InStream, class StreamWrapper, class Registry>
template <typename T, typename... Tn>
auto ReadArchive<InStream, StreamWrapper, Registry>::operator() (
    T& data, Tn&... data_n) -> ReadArchive&
{
    (*this) & data;
    return (*this)(data_n...);
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and meta::is_unsupported<T>())>
ReadArchive& operator& (ReadArchive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'siraf::ReadArchive'.");

    return archive;
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and not meta::is_registered<T>())>
ReadArchive& operator& (ReadArchive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unregistered type for the 'siraf::ReadArchive'.");

    return archive;
}

} // namespace siraf

#endif // SIRAF_READ_ARCHIVE_HPP
