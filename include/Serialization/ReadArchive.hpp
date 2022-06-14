#ifndef SERIALIZATION_READ_ARCHIVE_HPP
#define SERIALIZATION_READ_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map

#include <memory> // addressof

#include <Serialization/Access.hpp>
#include <Serialization/Registry.hpp>

#include <Serialization/Ref.hpp>
#include <Serialization/Span.hpp>

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
    using TrackingTable = std::unordered_map<std::uintptr_t, TrackingData>;

private:
    StreamWrapper archive_;
    TrackingTable track_table_;
    Registry registry_;

public:
    ReadArchive(InStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }
    auto tracking() noexcept -> TrackingTable& { return track_table_; }
    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T, meta::require<meta::is_arithmetic<T>()> = 0>
    auto operator& (T& number) -> ReadArchive&;

    template <typename T>
    auto operator>> (T& data) -> ReadArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> ReadArchive&;

private:
    auto operator() () -> ReadArchive& { return *this; }
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

template <class InStream, class Registry, class StreamWrapper>
template <typename T, typename... Tn>
auto ReadArchive<InStream, Registry, StreamWrapper>::operator() (T& data, Tn&... data_n) -> ReadArchive&
{
    (*this) & data;

    return (*this)(data_n...);
}

namespace tracking
{

template <class InStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<not meta::is_pointer<T>()> = 0>
void track(ReadArchive<InStream, Registry, StreamWrapper>& archive, T& data)
{
    using key_type =
        typename ReadArchive<InStream, Registry, StreamWrapper>::TrackingTable::key_type;

    key_type key;
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
    using key_type =
        typename ReadArchive<InStream, Registry, StreamWrapper>::TrackingTable::key_type;

    if (pointer != nullptr)
        throw "the read tracking pointer must be initialized to nullptr.";

    key_type key;
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

namespace detail
{

template <class InStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<not meta::is_span<T>()> = 0>
void raw_span(ReadArchive<InStream, Registry, StreamWrapper>& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class InStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<meta::is_span<T>()> = 0>
void raw_span(ReadArchive<InStream, Registry, StreamWrapper>& archive, T& zip)
{
    using size_type        = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    using pointer          = typename T::pointer;

    pointer ptr = new dereference_type [zip.size()];
    zip.init(ptr);

    for (size_type i = 0; i < zip.size(); ++i)
        raw_span(archive, zip[i]);
}

} // namespace detail

template <class InStream, class Registry, class StreamWrapper,
          typename T,
          typename D, typename... Dn,
          meta::require<meta::and_<std::is_arithmetic<D>,
                                   std::is_arithmetic<Dn>...>::value> = 0>
void span(ReadArchive<InStream, Registry, StreamWrapper>& archive,
           T& pointer, D& d, Dn&... dn)
{
    if (pointer != nullptr)
        throw "the read span data must be initialized to nullptr.";

    archive(d, dn...);

    auto span_data = zip(pointer, d, dn...);
    detail::raw_span(archive, span_data);
}

SERIALIZATION_READ_ARCHIVE_GENERIC(ref, meta::is_ref<T>())
{
    archive & ref.get();

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
    using key_type   = decltype(Access::template static_key<value_type>());

    key_type id;
    archive & id;

    Registry::load(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_READ_ARCHIVE_HPP
