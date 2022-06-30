#ifndef SERIALIZATION_WRITE_ARCHIVE_HPP
#define SERIALIZATION_WRITE_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map
#include <memory> // addressof

#include <Serialization/Access.hpp>
#include <Serialization//Registry.hpp>

#include <Serialization/Ref.hpp>
#include <Serialization/Span.hpp>

#include <Serialization/Detail/Tools.hpp>

#include <Serialization/Detail/Meta.hpp>

#define SERIALIZATION_WRITE_ARCHIVE_GENERIC(parameter, ...)                                             \
    template <class OutStream, class Registry, class StreamWrapper, typename T,                         \
              serialization::meta::require<(bool)(__VA_ARGS__)> = 0>                                    \
    auto operator& (                                                                                    \
        serialization::WriteArchive<OutStream, Registry, StreamWrapper>& archive,                       \
        T& parameter) -> decltype(archive)

namespace serialization
{

namespace utility
{

template <typename OutStream>
class OutStreamWrapper
{
private:
    OutStream& stream_;

public:
    OutStreamWrapper(OutStream& stream) : stream_(stream) {}

    template <typename T>
    OutStreamWrapper& write(const T& data, std::size_t n)
    {
        stream_.write(utility::const_byte_cast(data), n);

        return *this;
    }
};

} // namespace utility

template <class OutStream,
          class Registry = Registry<>,
          class StreamWrapper = utility::OutStreamWrapper<OutStream>>
class WriteArchive
{
public:
    using TrackingTable = std::unordered_map<std::uintptr_t, bool>;

private:
    StreamWrapper archive_;
    TrackingTable track_table_;
    Registry registry_;

public:
    WriteArchive(OutStream& stream);

    auto stream() noexcept -> StreamWrapper& { return archive_; }
    auto tracking() noexcept -> TrackingTable& { return track_table_; }
    auto registry() noexcept -> Registry& { return registry_; }

    template <typename T, meta::require<meta::is_arithmetic<T>()> = 0>
    auto operator& (T& number) -> WriteArchive&;

    template <typename T>
    auto operator<< (T& data) -> WriteArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> WriteArchive&;

private:
    auto operator() () -> WriteArchive& { return *this; }
};

namespace meta
{

template <typename> struct is_write_archive : std::false_type {};

template <class OutStream, class Registry, class StreamWrapper>
struct is_write_archive<WriteArchive<OutStream, Registry, StreamWrapper>> : std::true_type {};

} // namespace meta

template <class OutStream, class Registry, class StreamWrapper>
WriteArchive<OutStream, Registry, StreamWrapper>::WriteArchive(OutStream& stream)
    : archive_(stream), track_table_(), registry_()
{
}

template <class OutStream, class Registry, class StreamWrapper>
template <typename T, meta::require<meta::is_arithmetic<T>()>>
auto WriteArchive<OutStream, Registry, StreamWrapper>::operator& (T& number) -> WriteArchive&
{
    archive_.write(number, sizeof(number));

    return *this;
}

template <class OutStream, class Registry, class StreamWrapper>
template <typename T>
auto WriteArchive<OutStream, Registry, StreamWrapper>::operator<< (T& data) -> WriteArchive&
{
    return (*this) & data;
}

template <class OutStream, class Registry, class StreamWrapper>
template <typename T, typename... Tn>
auto WriteArchive<OutStream, Registry, StreamWrapper>::operator() (T& data, Tn&... data_n) -> WriteArchive&
{
    (*this) & data;

    return (*this)(data_n...);
}

namespace tracking
{

template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<not meta::is_pointer<T>()> = 0>
void track(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& data)
{
    using key_type =
        typename WriteArchive<OutStream, Registry, StreamWrapper>::TrackingTable::key_type;

    auto address = std::addressof(data);

    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.tracking()[key];

#ifdef SERIALIZATION_DEBUG
    if (is_tracking)
        throw "the write tracking data is already tracked.";
#endif
    is_tracking = true;

    archive & key;
    archive & data;
}

template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<meta::is_pointer<T>()> = 0>
void track(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& pointer)
{
    using key_type =
        typename WriteArchive<OutStream, Registry, StreamWrapper>::TrackingTable::key_type;

    auto key = reinterpret_cast<key_type>(pointer);

    auto& is_tracking = archive.tracking()[key];

    if (not is_tracking)
    {
        archive & key;
        archive & pointer; // call the serialization of not tracking pointer

        is_tracking = true;
    }
    else
    {
        archive & key;
    }
}

} // namespace tracking

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_WRITE_ARCHIVE_GENERIC(object, meta::is_unsupported<T>())
{
    static constexpr bool always_false = not meta::is_unsupported<T>();

    static_assert(always_false, "'T' is an unsupported type.");

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(object, Access::is_save_load_class<T>())
{
    Access::save(archive, object);

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(enumerator, meta::is_enum<T>())
{
    auto value = static_cast<typename std::underlying_type<T>::type>(enumerator);

    return archive & value;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(array, meta::is_array<T>())
{
    for (auto& item : array)
        archive & item;

    return archive;
}

namespace detail
{

template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<not meta::is_span<T>()> = 0>
void raw_span(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<meta::is_span<T>()> = 0>
void raw_span(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& zip)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < zip.size(); ++i)
        raw_span(archive, zip[i]);
}

} // namespace detail

template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          typename D, typename... Dn,
          meta::require<meta::and_<std::is_arithmetic<D>,
                                   std::is_arithmetic<Dn>...>::value> = 0>
void span(WriteArchive<OutStream, Registry, StreamWrapper>& archive,
           T& pointer, D& d, Dn&... dn)
{
    if (pointer == nullptr)
        throw "the write span data must be allocated.";

    archive(d, dn...);

    auto span_data = zip(pointer, d, dn...);
    detail::raw_span(archive, span_data);
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(ref, meta::is_ref<T>())
{
    if (ref.is_null())
        throw "the write reference cannot be null.";

    archive & ref.get();

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(pointer, meta::is_pod_pointer<T>())
{
    if (pointer == nullptr)
        throw "the write pointer must be allocated.";

    archive & (*pointer);

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(pointer, meta::is_polymorphic_pointer<T>())
{
    if (pointer == nullptr)
        throw "the write pointer was not allocated.";

    auto id = Access::dynamic_key(*pointer);
    archive & id;

    Registry::save(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_WRITE_ARCHIVE_HPP
