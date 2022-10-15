#ifndef SIFAR_WRITE_ARCHIVE_HPP
#define SIFAR_WRITE_ARCHIVE_HPP

#include <cstdint> // uintptr_t
#include <cstddef> // size_t
#include <unordered_map> // unordered_map
#include <memory> // addressof

#include <Sifar/ArchiveBase.hpp>

#include <Sifar/Access.hpp>
#include <Sifar/TypeRegistry.hpp>
#include <Sifar/Registry.hpp>

#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define SERIALIZATION_SAVE_DATA(parameter, ...)                                                         \
    template <class WriteArchive, typename T,                                                           \
              SIFAR_REQUIRE(::sifar::meta::is_write_archive<WriteArchive>() and                         \
                            ::sifar::meta::is_registered<T>() and                                       \
                            (bool)(__VA_ARGS__))>                                                       \
    WriteArchive& operator& (WriteArchive& archive, T& parameter)

namespace sifar
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
    OutStreamWrapper& write(const T* data, std::size_t n)
    {
        stream_.write(utility::const_byte_cast(data), n);

        return *this;
    }

    OutStream& get() noexcept { return stream_; }
};

} // namespace wrap

template <class OutStream,
          class StreamWrapper = wrapper::OutStreamWrapper<OutStream>,
          class Registry = dynamic::ExternRegistry>
class WriteArchive : public core::ArchiveBase
{
    SERIALIZATION_ARCHIVE(WriteArchive)

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

    auto operator() () noexcept -> WriteArchive& { return *this; }
};

template <class OutStream>
WriteArchive<OutStream> writer(OutStream& stream)
{
    return { stream };
}

template <template <class, typename...> class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class OutStream>
WriteArchive<OutStream, StreamWrapper<OutStream>, Registry> writer(OutStream& stream)
{
    return { stream };
}

template <class StreamWrapper,
          class Registry = dynamic::ExternRegistry,
          class OutStream>
WriteArchive<OutStream, StreamWrapper, Registry> writer(OutStream& stream)
{
    return { stream };
}

template <class OutStream, class StreamWrapper, class Registry>
WriteArchive<OutStream, StreamWrapper, Registry>::WriteArchive(OutStream& stream)
    : archive_{stream}, track_table_(), registry_()
{
}

template <class OutStream, class StreamWrapper, class Registry>
template <typename T>
auto WriteArchive<OutStream, StreamWrapper, Registry>::operator<< (
    T& data) -> WriteArchive&
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
          meta::require<meta::is_write_archive<WriteArchive>()
                        and meta::is_unsupported<T>()> = 0>
WriteArchive& operator& (WriteArchive& archive, T& unsupported)
{
    static_assert(meta::to_false<T>(),
        "'T' is an unsupported type for the 'sifar::WriteArchive'.");

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
        "and then register the type 'T' with the macros: "
        "'SERIALIZATION_TYPE_REGISTRY(name)' or "
        "'SERIALIZATION_TYPE_REGISTRY_IF(condition)'.");

    return archive;
}

// inline namespace common also used in namespace library
inline namespace common
{

SERIALIZATION_SAVE_DATA(object, Access::is_save_class<T>())
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
    using underlying_type = typename std::underlying_type<T>::type;
    auto value = static_cast<underlying_type>(enumerator);

    return archive & value;
}

SERIALIZATION_SAVE_DATA(array, meta::is_array<T>())
{
    for (const auto& item : array)
        archive & item;

    return archive;
}

SERIALIZATION_SAVE_DATA(pod_pointer, meta::is_pod_pointer<T>())
{
    if (pod_pointer == nullptr)
        throw "the write pointer must be allocated.";

    archive & (*pod_pointer);

    return archive;
}

SERIALIZATION_SAVE_DATA(pointer, meta::is_pointer_to_polymorphic<T>())
{
    auto& registry = archive.registry();

    auto id = registry.save_key(archive, pointer);
    registry.save(archive, pointer, id);

    return archive;
}

} // inline namespace common

} // namespace sifar

#endif // SIFAR_WRITE_ARCHIVE_HPP
