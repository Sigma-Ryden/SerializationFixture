#ifndef SERIALIZATION_WRITE_ARCHIVE_HPP
#define SERIALIZATION_WRITE_ARCHIVE_HPP

#include <cstddef> // size_t
#include <unordered_map> // unordered_map
#include <memory> // addressof

#include <Serialization/Access.hpp>
#include <Serialization//Registry.hpp>

#include <Serialization/Scope.hpp>

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
    using registry = Registry;
    using TrackTable = std::unordered_map<std::size_t, bool>;

private:
    StreamWrapper archive_;
    TrackTable track_table_;

public:
    WriteArchive(OutStream& stream);

    auto stream() noexcept -> StreamWrapper&;
    auto tracking() noexcept -> TrackTable&;

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
    : archive_(stream), track_table_()
{
}

template <class Registry, class OutStream, class StreamWrapper>
auto WriteArchive<Registry, OutStream, StreamWrapper>::stream() noexcept -> StreamWrapper&
{
    return archive_;
}

template <class Registry, class OutStream, class StreamWrapper>
auto WriteArchive<Registry, OutStream, StreamWrapper>::tracking() noexcept -> TrackTable&
{
    return track_table_;
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

template <class OutStream, class Registry, class StreamWrapper,
          typename T>
void track(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& data)
{
    auto address = std::addressof(data);

    auto key = reinterpret_cast<std::size_t>(address);

    archive.tracking()[key] = true;

    archive & key;
    archive & data;
}

template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<meta::is_pointer<T>()> = 0>
void track_partial(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& pointer)
{
    auto key = reinterpret_cast<std::size_t>(pointer);

    bool& is_tracking = archive.tracking()[key];

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

template <class OutStream, class Registry, class StreamWrapper,
          typename T,
          meta::require<meta::is_pointer<T>()> = 0>
void track_always(WriteArchive<OutStream, Registry, StreamWrapper>& archive, T& pointer)
{
    auto key = reinterpret_cast<std::size_t>(pointer);

    archive.tracking()[key] = true; // explicit data tracking

    archive & key;
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

SERIALIZATION_WRITE_ARCHIVE_GENERIC(scope, meta::is_scope<T>())
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

    auto index = Registry::index(pointer);
    archive & index;

    Registry::save(archive, pointer, index);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_WRITE_ARCHIVE_HPP
