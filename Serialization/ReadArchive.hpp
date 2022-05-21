#ifndef SERIALIZATION_READ_ARCHIVE_HPP
#define SERIALIZATION_READ_ARCHIVE_HPP

#include <cstddef> // size_t

#include "Access.hpp"
#include "Scope.hpp"

#include "Detail/Tools.hpp"

#include "Detail/Meta.hpp"
#include "Detail/TypeDetection.hpp"

#include "Detail/MacroScope.hpp"

#define SERIALIZATION_READ_ARCHIVE_GENERIC(parameter, ...)                                              \
    template <class InStream, class StreamWrapper, typename T,                                          \
              serialization::meta::require<(bool)(__VA_ARGS__)> = 0>                                    \
    auto operator& (                                                                                    \
        serialization::ReadArchive<InStream, StreamWrapper>& archive,                                   \
        T& parameter) -> decltype(archive)

namespace serialization
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
        stream_.read(detail::byte_cast(data), n);

        return *this;
    }
};

template <class InStream, class StreamWrapper = InStreamWrapper<InStream>>
class ReadArchive
{
private:
    StreamWrapper archive_;

public:
    ReadArchive(InStream& stream);

    auto stream() -> StreamWrapper&;

    template <typename T, meta::require<meta::is_arithmetic<T>()> = 0>
    auto operator& (T& number) -> ReadArchive&;

    template <typename T>
    auto operator>> (T& data) -> ReadArchive&;
};

template <class InStream, class StreamWrapper>
ReadArchive<InStream, StreamWrapper>::ReadArchive(InStream& stream)
    : archive_(stream)
{
}

template <class InStream, class StreamWrapper>
auto ReadArchive<InStream, StreamWrapper>::stream() -> StreamWrapper&
{
    return archive_;
}

template <class InStream, class StreamWrapper>
template <typename T, meta::require<meta::is_arithmetic<T>()>>
auto ReadArchive<InStream, StreamWrapper>::operator& (T& number) -> ReadArchive&
{
    archive_.read(number, sizeof(number));

    return *this;
}

template <class InStream, class StreamWrapper>
template <typename T>
auto ReadArchive<InStream, StreamWrapper>::operator>> (T& data) -> ReadArchive&
{
    return (*this) & data;
}

// Class Template Argument Deduction by make function
template <class InStream>
ReadArchive<InStream> read_archive(InStream& archive)
{
    return { archive };
}

namespace support
{

namespace common
{

SERIALIZATION_READ_ARCHIVE_GENERIC(object, Access::is_serialize_class<T>())
{
    Access::serialize(archive, object);

    return archive;
}

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

// will be change
SERIALIZATION_READ_ARCHIVE_GENERIC(pointer, meta::is_pointer<T>())
{
    using value_type = meta::deref<T>;

    if (pointer == nullptr)
        throw "load pointer was not allocated.";

    value_type item;

    archive & item;

    new (pointer) value_type(std::move(item));

    return archive;
}

} // namespace common

namespace library
{

SERIALIZATION_READ_ARCHIVE_GENERIC(string, meta::is_std_basic_string<T>())
{
    using size_type = typename T::size_type;

    size_type string_size = 0;

    archive & string_size;

    string.resize(string_size);

    for(auto& character : string)
        archive & character;

    return archive;
}

} // library

} // namespace support

} // namespace serialization

#include "Detail/MacroUnscope.hpp"

#endif // SERIALIZATION_READ_ARCHIVE_HPP
