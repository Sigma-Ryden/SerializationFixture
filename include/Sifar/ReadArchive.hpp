#ifndef SIFAR_READ_ARCHIVE_HPP
#define SIFAR_READ_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map

#include <memory> // addressof

#include <Sifar/Access.hpp>
#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Registry.hpp>

#include <Sifar/Ref.hpp>
#include <Sifar/Span.hpp>

#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>

#define SERIALIZATION_LOAD_DATA(parameter, ...)                                                         \
    template <class ReadArchive, typename T,                                                            \
              ::sifar::meta::require<::sifar::meta::is_read_archive<ReadArchive>() and                  \
                                     ::sifar::meta::is_registered<T>() and                              \
                                     (bool)(__VA_ARGS__)> = 0>                                          \
    ReadArchive& operator& (ReadArchive& archive, T& parameter)

namespace sifar
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
    InStreamWrapper& read(T* data, std::size_t n)
    {
        stream_.read(utility::byte_cast(data), n);

        return *this;
    }

    InStream& get() noexcept { return stream_; }
};

} // namespace utility

template <class InStream,
          class Registry = ExternRegistry,
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

    template <typename T>
    auto operator>> (T& data) -> ReadArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> ReadArchive&;

    auto operator() () -> ReadArchive& { return *this; }
};

namespace meta
{

namespace detail
{

template <typename> struct is_read_archive : std::false_type {};

template <class InStream, class Registry, class StreamWrapper>
struct is_read_archive<ReadArchive<InStream, Registry, StreamWrapper>> : std::true_type {};

} // namespace detail

template <class T> constexpr bool is_read_archive()
{
    return detail::is_read_archive<T>::value;
}

} // namespace meta

template <class InStream, class Registry, class StreamWrapper>
ReadArchive<InStream, Registry, StreamWrapper>::ReadArchive(InStream& stream)
    : archive_(stream), track_table_(), registry_()
{
}

template <class InStream, class Registry, class StreamWrapper>
template <typename T>
auto ReadArchive<InStream, Registry, StreamWrapper>::operator>> (
    T& data) -> ReadArchive&
{
    return (*this) & data;
}

template <class InStream, class Registry, class StreamWrapper>
template <typename T, typename... Tn>
auto ReadArchive<InStream, Registry, StreamWrapper>::operator() (
    T& data, Tn&... data_n) -> ReadArchive&
{
    (*this) & data;

    return (*this)(data_n...);
}

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and meta::is_unsupported<T>()> = 0>
ReadArchive& operator& (ReadArchive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "'T' is an unsupported type for the 'sifar::ReadArchive'."
    );

    return archive;
}

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and not meta::is_registered<T>()> = 0>
ReadArchive& operator& (ReadArchive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        "'T' is an unregistered type for the 'sifar::ReadArchive'. "
        "Try overload an operator& to serialize the type 'T' with the macro "
        "'SERIALIZATION_LOAD_DATA(parameter, condition)' "
        "and then register the type 'T' with the macros: "
        "'SERIALIZATION_TYPE_REGISTRY(name)' or "
        "'SERIALIZATION_TYPE_REGISTRY_IF(condition)'."
    );

    return archive;
}

namespace detail
{

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and not meta::is_span<T>()> = 0>
void raw_span(ReadArchive& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and meta::is_span<T>()> = 0>
void raw_span(ReadArchive& archive, T& zip)
{
    using size_type        = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    using pointer          = typename T::pointer;

    pointer ptr = new dereference_type [zip.size()];
    zip.init(ptr);

    for (size_type i = 0; i < zip.size(); ++i)
        raw_span(archive, zip[i]);
}

template <class Archive, typename T,
          meta::require<meta::is_read_archive<Archive>()
                        and not Access::is_registered_class<meta::deref<T>>()> = 0>
void native_load(Archive& archive, T& pointer, void* address)
{
    pointer = static_cast<T>(address);
}

template <class Archive, typename T,
          meta::require<meta::is_read_archive<Archive>()
                        and Access::is_registered_class<meta::deref<T>>()> = 0>
void native_load(Archive& archive, T& pointer, void* address)
{
    auto id = archive.registry().load_key(archive, pointer);

    archive.registry().assign(pointer, address, id);
}

} // namespace detail

namespace tracking
{

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and not meta::is_pointer<T>()> = 0>
void track(ReadArchive& archive, T& data)
{
    using key_type = typename ReadArchive::TrackingTable::key_type;

    key_type key;
    archive & key;

    auto& track_data = archive.tracking()[key];

#ifdef SIFAR_DEBUG
    if (track_data.is_tracking)
        throw  "the read tracking data is already tracked.";
#endif
    auto address = utility::pure(std::addressof(data));

    track_data.address = address;
    track_data.is_tracking = true;

    archive & data;
}

template <class ReadArchive, typename T,
          meta::require<meta::is_read_archive<ReadArchive>()
                        and meta::is_pointer<T>()> = 0>
void track(ReadArchive& archive, T& pointer)
{
    using key_type = typename ReadArchive::TrackingTable::key_type;

    if (pointer != nullptr)
        throw "the read tracking pointer must be initialized to nullptr.";

    key_type key;
    archive & key;

    auto& track_data = archive.tracking()[key];

    if (not track_data.is_tracking)
    {
        archive & pointer; // call the serialization of not tracking pointer

        track_data.address = utility::pure(pointer);
        track_data.is_tracking = true;
    }
    else
    {
        detail::native_load(archive, pointer, track_data.address);
    }
}

} // namespace tracking

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_LOAD_DATA(object, Access::is_save_load_class<T>())
{
    Access::load(archive, object);

    return archive;
}

SERIALIZATION_LOAD_DATA(number, meta::is_arithmetic<T>())
{
    archive.stream().read(&number, sizeof(number));

    return archive;
}

SERIALIZATION_LOAD_DATA(enumerator, meta::is_enum<T>())
{
    using underlying_type = typename std::underlying_type<T>::type;

    underlying_type buff = 0;

    archive & buff;

    enumerator = static_cast<T>(buff);

    return archive;
}

SERIALIZATION_LOAD_DATA(array, meta::is_array<T>())
{
    for (auto& item : array)
        archive & item;

    return archive;
}

template <class ReadArchive, typename T,
          typename D, typename... Dn,
          meta::require<meta::is_read_archive<ReadArchive>() and
                        meta::and_<std::is_arithmetic<D>,
                                   std::is_arithmetic<Dn>...>::value> = 0>
void span(ReadArchive& archive, T& pointer, D& d, Dn&... dn)
{
    if (pointer != nullptr)
        throw "the read span data must be initialized to nullptr.";

    archive(d, dn...);

    auto span_data = zip(pointer, d, dn...);
    detail::raw_span(archive, span_data);
}

SERIALIZATION_LOAD_DATA(ref, meta::is_ref<T>())
{
    archive & ref.get();

    return archive;
}

SERIALIZATION_LOAD_DATA(pointer, meta::is_pod_pointer<T>())
{
    using value_type = meta::deref<T>;

    if (pointer != nullptr)
        throw "the read pointer must be initialized to nullptr.";

    pointer = new value_type;

    archive & (*pointer);

    return archive;
}

SERIALIZATION_LOAD_DATA(pointer, meta::is_pointer_to_polymorphic<T>())
{
    auto id = archive.registry().load_key(archive, pointer);

    archive.registry().load(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace sifar

#endif // SIFAR_READ_ARCHIVE_HPP
