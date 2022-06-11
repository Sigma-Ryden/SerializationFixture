#ifndef SERIALIZATION_WRITE_ARCHIVE_HPP
#define SERIALIZATION_WRITE_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map
#include <memory> // addressof

#include <Serialization/Access.hpp>
#include <Serialization//Registry.hpp>

#include <Serialization/Ref.hpp>
#include <Serialization/Scope.hpp>

#include <Serialization/Detail/Tools.hpp>

#include <Serialization/Detail/Meta.hpp>

#define SERIALIZATION_WRITE_ARCHIVE_GENERIC_HELPER(function_name, parameter_name, ...)                  \
    template <class OutStream, class Registry, class StreamWrapper, typename T,                         \
              serialization::meta::require<(bool)(__VA_ARGS__)> = 0>                                    \
    auto function_name(                                                                                 \
        serialization::WriteArchive<OutStream, Registry, StreamWrapper>& archive,                       \
        T& parameter_name) -> decltype(archive)

#define SERIALIZATION_WRITE_ARCHIVE_GENERIC(parameter_name, ...)                                        \
    SERIALIZATION_WRITE_ARCHIVE_GENERIC_HELPER(                                                         \
        operator&,                                                                                      \
        parameter_name,                                                                                 \
        __VA_ARGS__)

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

    auto stream() noexcept -> StreamWrapper&;
    auto tracking() noexcept -> TrackingTable&;
    auto registry() noexcept -> Registry&;

    template <typename T, meta::require<meta::is_arithmetic<T>()> = 0>
    auto operator& (T& number) -> WriteArchive&;

    template <typename T>
    auto operator<< (T& data) -> WriteArchive&;
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
auto WriteArchive<OutStream, Registry, StreamWrapper>::stream() noexcept -> StreamWrapper&
{
    return archive_;
}

template <class OutStream, class Registry, class StreamWrapper>
auto WriteArchive<OutStream, Registry, StreamWrapper>::tracking() noexcept -> TrackingTable&
{
    return track_table_;
}

template <class OutStream, class Registry, class StreamWrapper>
auto WriteArchive<OutStream, Registry, StreamWrapper>::registry() noexcept -> Registry&
{
    return registry_;
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

namespace tracking
{

SERIALIZATION_WRITE_ARCHIVE_GENERIC_HELPER(track, data, not meta::is_pointer<T>())
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

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC_HELPER(track, pointer, meta::is_pointer<T>())
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

    return archive;
}

} // namespace tracking

// inline namespace common also used in namespace library
inline namespace common
{

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

SERIALIZATION_WRITE_ARCHIVE_GENERIC_HELPER(scope, data, not meta::is_scope<T>())
{
    archive & data;

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC_HELPER(scope, zip, meta::is_scope<T>())
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < zip.size(); ++i)
        scope(archive, zip[i]);

    return archive;
}

} // namespace detail

SERIALIZATION_WRITE_ARCHIVE_GENERIC(scope, meta::is_scope<T>())
{
    archive & scope.dim();

    detail::scope(archive, scope);

    return archive;
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

    auto index = Access::dynamic_key(*pointer);
    archive & index;

    Registry::save(archive, pointer, index);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_WRITE_ARCHIVE_HPP
