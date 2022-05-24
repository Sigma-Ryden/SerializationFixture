#ifndef SERIALIZATION_WRITE_ARCHIVE_HPP
#define SERIALIZATION_WRITE_ARCHIVE_HPP

#include <cstddef> // size_t

#include "Access.hpp"
#include "Scope.hpp"

#include "Detail/Tools.hpp"

#include "Detail/Meta.hpp"

#define SERIALIZATION_WRITE_ARCHIVE_GENERIC(parameter, ...)                                             \
    template <class Registry, class OutStream, class StreamWrapper, typename T,                         \
              serialization::meta::require<(bool)(__VA_ARGS__)> = 0>                                    \
    auto operator& (                                                                                    \
        serialization::WriteArchive<Registry, OutStream, StreamWrapper>& archive,                       \
        T& parameter) -> decltype(archive)

namespace serialization
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
        stream_.write(detail::const_byte_cast(data), n);

        return *this;
    }
};

template <class Registry, class OutStream, class StreamWrapper = OutStreamWrapper<OutStream>>
class WriteArchive
{
public:
    using registry = Registry;

private:
    StreamWrapper archive_;

public:
    WriteArchive(OutStream& stream);

    auto stream() -> StreamWrapper&;

    template <typename T, meta::require<meta::is_arithmetic<T>()> = 0>
    auto operator& (T& number) -> WriteArchive&;

    template <typename T>
    auto operator<< (T& data) -> WriteArchive&;
};

template <class Registry, class OutStream, class StreamWrapper>
WriteArchive<Registry, OutStream, StreamWrapper>::WriteArchive(OutStream& stream)
    : archive_(stream)
{
}

template <class Registry, class OutStream, class StreamWrapper>
auto WriteArchive<Registry, OutStream, StreamWrapper>::stream() -> StreamWrapper&
{
    return archive_;
}

template <class Registry, class OutStream, class StreamWrapper>
template <typename T, meta::require<meta::is_arithmetic<T>()>>
auto WriteArchive<Registry, OutStream, StreamWrapper>::operator& (T& number) -> WriteArchive&
{
    archive_.write(number, sizeof(number));

    return *this;
}

template <class Registry, class OutStream, class StreamWrapper>
template <typename T>
auto WriteArchive<Registry, OutStream, StreamWrapper>::operator<< (T& data) -> WriteArchive&
{
    return (*this) & data;
}
/*
namespace make
{

// Class Template Argument Deduction by make function
template <class OutStream>
WriteArchive<OutStream> write_archive(OutStream& archive)
{
    return { archive };
}

} // namespace make
*/
// inline namespace common also used in namespace library
inline namespace common
{
/*
SERIALIZATION_WRITE_ARCHIVE_GENERIC(object, Access::is_serialize_class<T>())
{
    Access::serialize(archive, object);

    return archive;
}
*/
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
        throw "save pointer was not allocated.";

    archive & (*pointer);

    return archive;
}

SERIALIZATION_WRITE_ARCHIVE_GENERIC(pointer, meta::is_polymorphic_pointer<T>())
    // or meta::is_abstract_pointer<T>())
{
    if (pointer == nullptr)
        throw "save pointer was not allocated.";

    auto index = Registry::index(pointer);
    archive & index;

    Registry::save(archive, pointer, index);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_WRITE_ARCHIVE_HPP
