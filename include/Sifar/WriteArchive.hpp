#ifndef SIFAR_WRITE_ARCHIVE_HPP
#define SIFAR_WRITE_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map
#include <memory> // addressof

#include <Sifar/Access.hpp>
#include <Sifar/TypeRegistry.hpp>

#include <Sifar//Registry.hpp>

#include <Sifar/Ref.hpp>
#include <Sifar/Span.hpp>

#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>

#define SERIALIZATION_SAVE_DATA(parameter, ...)                                                         \
    template <class WriteArchive, typename T,                                                           \
              ::sifar::meta::require<::sifar::meta::is_write_archive<WriteArchive>() and                \
                                     ::sifar::meta::is_registered<T>() and                              \
                                     (bool)(__VA_ARGS__)> = 0>                                          \
    WriteArchive& operator& (WriteArchive& archive, T& parameter)

namespace sifar
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
    OutStreamWrapper& write(const T* data, std::size_t n)
    {
        stream_.write(utility::const_byte_cast(data), n);

        return *this;
    }

    OutStream& get() noexcept { return stream_; }
};

} // namespace utility

template <class OutStream,
          class Registry = ExternRegistry,
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

    template <typename T>
    auto operator<< (T& data) -> WriteArchive&;

    template <typename T, typename... Tn>
    auto operator() (T& data, Tn&... data_n) -> WriteArchive&;

    auto operator() () -> WriteArchive& { return *this; }
};

namespace meta
{

namespace detail
{

template <typename> struct is_write_archive : std::false_type {};

template <class OutStream, class Registry, class StreamWrapper>
struct is_write_archive<WriteArchive<OutStream, Registry, StreamWrapper>> : std::true_type {};

} // namespace detail

template <class T> constexpr bool is_write_archive()
{
    return detail::is_write_archive<T>::value;
}

} // namespace meta

template <class OutStream, class Registry, class StreamWrapper>
WriteArchive<OutStream, Registry, StreamWrapper>::WriteArchive(OutStream& stream)
    : archive_(stream), track_table_(), registry_()
{
}

template <class OutStream, class Registry, class StreamWrapper>
template <typename T>
auto WriteArchive<OutStream, Registry, StreamWrapper>::operator<< (
    T& data) -> WriteArchive&
{
    return (*this) & data;
}

template <class OutStream, class Registry, class StreamWrapper>
template <typename T, typename... Tn>
auto WriteArchive<OutStream, Registry, StreamWrapper>::operator() (
    T& data, Tn&... data_n) -> WriteArchive&
{
    (*this) & data;

    return (*this)(data_n...);
}

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and meta::is_unsupported<T>()> = 0>
WriteArchive& operator& (WriteArchive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "'T' is an unsupported type for the 'sifar::WriteArchive'."
    );

    return archive;
}

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and not meta::is_registered<T>()> = 0>
WriteArchive& operator& (WriteArchive& archive, T& unregistered)
{
    static_assert(meta::to_false<T>(),
        "'T' is an unregistered type for the 'sifar::WriteArchive'. "
        "Try overload an operator& to serialize the type 'T' with the macro "
        "'SERIALIZATION_SAVE_DATA(parameter, condition)' "
        "and then register the type 'T' with the macros "
        "'SERIALIZATION_TYPE_REGISTRY(name)' or "
        "'SERIALIZATION_TYPE_REGISTRY_IF(condition)'."
    );

    return archive;
}

namespace detail
{

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and not meta::is_span<T>()> = 0>
void raw_span(WriteArchive& archive, T& data)
{
    archive & data;
}

// sifar of scoped data with previous dimension initialization
template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and meta::is_span<T>()> = 0>
void raw_span(WriteArchive& archive, T& zip)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < zip.size(); ++i)
        raw_span(archive, zip[i]);
}

template <class Archive, typename T, typename key_type,
          meta::require<meta::is_write_archive<Archive>()
                        and not Access::is_registered_class<meta::deref<T>>()> = 0>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
}

template <class Archive, typename T, typename key_type,
          meta::require<meta::is_write_archive<Archive>()
                        and Access::is_registered_class<meta::deref<T>>()> = 0>
void native_save(Archive& archive, T& pointer, key_type track_key)
{
    archive & track_key;
    archive.registry().save_key(archive, pointer); // write class info
}

} // namespace detail

namespace tracking
{

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and not meta::is_pointer<T>()> = 0>
void track(WriteArchive& archive, T& data)
{
    using key_type = typename WriteArchive::TrackingTable::key_type;

    auto address = utility::pure(std::addressof(data));

    auto key = reinterpret_cast<key_type>(address);

    auto& is_tracking = archive.tracking()[key];

#ifdef SIFAR_DEBUG
    if (is_tracking)
        throw "the write tracking data is already tracked.";
#endif
    is_tracking = true;

    archive & key;
    archive & data;
}

template <class WriteArchive, typename T,
          meta::require<meta::is_write_archive<WriteArchive>()
                        and meta::is_pointer<T>()> = 0>
void track(WriteArchive& archive, T& pointer)
{
    using key_type = typename WriteArchive::TrackingTable::key_type;

    auto key = reinterpret_cast<key_type>(utility::pure(pointer));

    auto& is_tracking = archive.tracking()[key];

    if (not is_tracking)
    {
        archive & key;
        archive & pointer; // call the sifar of not tracking pointer

        is_tracking = true;
    }
    else
    {
        detail::native_save(archive, pointer, key);
    }
}

} // namespace tracking

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_SAVE_DATA(object, Access::is_save_load_class<T>())
{
    Access::save(archive, object);

    return archive;
}

SERIALIZATION_SAVE_DATA(number, meta::is_arithmetic<T>())
{
    archive.stream().write(&number, sizeof(number));

    return archive;
}

SERIALIZATION_SAVE_DATA(enumerator, meta::is_enum<T>())
{
    auto value = static_cast<typename std::underlying_type<T>::type>(enumerator);

    return archive & value;
}

SERIALIZATION_SAVE_DATA(array, meta::is_array<T>())
{
    for (auto& item : array)
        archive & item;

    return archive;
}

template <class WriteArchive, typename T,
          typename D, typename... Dn,
          meta::require<meta::is_write_archive<WriteArchive>() and
                        meta::and_<std::is_arithmetic<D>,
                                   std::is_arithmetic<Dn>...>::value> = 0>
void span(WriteArchive& archive, T& pointer, D& d, Dn&... dn)
{
    if (pointer == nullptr)
        throw "the write span data must be allocated.";

    archive(d, dn...);

    auto span_data = zip(pointer, d, dn...);
    detail::raw_span(archive, span_data);
}

SERIALIZATION_SAVE_DATA(ref, meta::is_ref<T>())
{
    if (ref.is_null())
        throw "the write reference cannot be null.";

    archive & ref.get();

    return archive;
}

SERIALIZATION_SAVE_DATA(pointer, meta::is_pod_pointer<T>())
{
    if (pointer == nullptr)
        throw "the write pointer must be allocated.";

    archive & (*pointer);

    return archive;
}

SERIALIZATION_SAVE_DATA(pointer, meta::is_pointer_to_polymorphic<T>())
{
    auto id = archive.registry().save_key(archive, pointer);

    archive.registry().save(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace sifar

#endif // SIFAR_WRITE_ARCHIVE_HPP
