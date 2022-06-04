#ifndef SERIALIZATION_READ_ARCHIVE_HPP
#define SERIALIZATION_READ_ARCHIVE_HPP

#include <cstddef> // size_t
#include <unordered_map> // unordered_map

#include <memory> // addressof

#include <Serialization/Access.hpp>
#include <Serialization/Registry.hpp>

#include <Serialization/Scope.hpp>

#include <Serialization/Detail/Tools.hpp>

#include <Serialization/Detail/Meta.hpp>

#define SERIALIZATION_READ_ARCHIVE_GENERIC(parameter, ...)                                              \
    template <class InStream, class Registry, class StreamWrapper, typename T,                          \
              serialization::meta::require<(bool)(__VA_ARGS__)> = 0>                                    \
    auto operator& (                                                                                    \
        serialization::ReadArchive<InStream, Registry, StreamWrapper>& archive,                         \
        T& parameter) -> decltype(archive)

namespace serialization
{

namespace utility
{

template <typename InStream>
class InStreamWrapper
{
private:
    InStream& stream_;

public:
    InStreamWrapper(InStream& stream) : stream_(stream) {}

    template <typename T>
    InStreamWrapper& read(T& data, std::size_t n)
    {
        stream_.read(utility::byte_cast(data), n);

        return *this;
    }
};

} // namespace utility

template <class InStream,
          class Registry = Registry<>,
          class StreamWrapper = utility::InStreamWrapper<InStream>>
class ReadArchive
{
private:
    struct TrackingData
    {
        void* address = nullptr;
        bool is_tracking = false;
    };

public:
    using TrackingTable = std::unordered_map<std::size_t, TrackingData>;

private:
    StreamWrapper archive_;
    TrackingTable track_table_;
    Registry registry_;

public:
    ReadArchive(InStream& stream);

    auto stream() noexcept -> StreamWrapper&;
    auto tracking() noexcept -> TrackingTable&;
    auto registry() noexcept -> Registry&;

    template <typename T, meta::require<meta::is_arithmetic<T>()> = 0>
    auto operator& (T& number) -> ReadArchive&;

    template <typename T>
    auto operator>> (T& data) -> ReadArchive&;
};

namespace meta
{

template <typename> struct is_read_archive : std::false_type {};

template <class InStream, class Registry, class StreamWrapper>
struct is_read_archive<ReadArchive<InStream, Registry, StreamWrapper>> : std::true_type {};

} // namespace meta

template <class InStream, class Registry, class StreamWrapper>
ReadArchive<InStream, Registry, StreamWrapper>::ReadArchive(InStream& stream)
    : archive_(stream), track_table_(), registry_()
{
}

template <class InStream, class Registry, class StreamWrapper>
auto ReadArchive<InStream, Registry, StreamWrapper>::stream() noexcept -> StreamWrapper&
{
    return archive_;
}

template <class InStream, class Registry, class StreamWrapper>
auto ReadArchive<InStream, Registry, StreamWrapper>::tracking() noexcept -> TrackingTable&
{
    return track_table_;
}

template <class InStream, class Registry, class StreamWrapper>
auto ReadArchive<InStream, Registry, StreamWrapper>::registry() noexcept -> Registry&
{
    return registry_;
}

template <class InStream, class Registry, class StreamWrapper>
template <typename T, meta::require<meta::is_arithmetic<T>()>>
auto ReadArchive<InStream, Registry, StreamWrapper>::operator& (T& number) -> ReadArchive&
{
    archive_.read(number, sizeof(number));

    return *this;
}

template <class InStream, class Registry, class StreamWrapper>
template <typename T>
auto ReadArchive<InStream, Registry, StreamWrapper>::operator>> (T& data) -> ReadArchive&
{
    return (*this) & data;
}

namespace tracking
{

template <class InStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<not meta::is_pointer<T>()> = 0>
void track(ReadArchive<InStream, Registry, StreamWrapper>& archive, T& data)
{
    std::size_t key;
    archive & key;

    auto& track_data = archive.tracking()[key];

#ifdef SERIALIZATION_DEBUG
    if (track_data.is_tracking)
        throw  "the read tracking data is already tracked.";
#endif
    auto address = std::addressof(data);

    track_data.address = address;
    track_data.is_tracking = true;

    archive & data;
}

template <class InStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<meta::is_pointer<T>()> = 0>
void track(ReadArchive<InStream, Registry, StreamWrapper>& archive, T& pointer)
{
    if (pointer != nullptr)
        throw "the read tracking pointer must be initialized to nullptr.";

    std::size_t key;
    archive & key;

    auto& track_data = archive.tracking()[key];

    if (not track_data.is_tracking)
    {
        archive & pointer; // call the serialization of not tracking pointer

        track_data.address = pointer;
        track_data.is_tracking = true;
    }
    else
    {
        pointer = static_cast<T>(track_data.address);
    }
}

} // namespace tracking

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_READ_ARCHIVE_GENERIC(object, Access::is_save_load_class<T>())
{
    Access::load(archive, object);

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff = 0;

    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(array, meta::is_array<T>())
{
    for (auto& item : array)
        archive & item;

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(scope, meta::is_scope<T>())
{
    auto first = scope.data();
    auto last  = scope.data() + scope.size();

    while(first != last)
    {
        archive & (*first);
        ++first;
    }

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(pointer, meta::is_pod_pointer<T>())
{
    using value_type = meta::deref<T>;

    if (pointer != nullptr)
        throw "the read pointer must be initialized to nullptr.";

    pointer = new value_type;

    archive & (*pointer);

    return archive;
}

SERIALIZATION_READ_ARCHIVE_GENERIC(pointer, meta::is_polymorphic_pointer<T>())
{
    using value_type = meta::deref<T>;
    using index_type = decltype(value_type::static_index());

    index_type id;
    archive & id;

    Registry::load(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_READ_ARCHIVE_HPP
