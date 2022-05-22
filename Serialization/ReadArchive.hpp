#ifndef SERIALIZATION_READ_ARCHIVE_HPP
#define SERIALIZATION_READ_ARCHIVE_HPP

#include <cstddef> // size_t

#include <memory> // operator new

#include "Access.hpp"
#include "Scope.hpp"

#include "Detail/Tools.hpp"

#include "Detail/Meta.hpp"

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

namespace make
{

// Class Template Argument Deduction by make function
template <class InStream>
ReadArchive<InStream> read_archive(InStream& archive)
{
    return { archive };
}

} // namespace make

// inline namespace common also used in namespace library
inline namespace common
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

// will be change
SERIALIZATION_READ_ARCHIVE_GENERIC(pointer, meta::is_pointer<T>())
{
    using value_type = meta::deref<T>;

    // this will not work correct if object was serialized on stack
    pointer = new value_type;

    archive & (*pointer);

    return archive;
}

} // inline namespace common

} // namespace serialization

#endif // SERIALIZATION_READ_ARCHIVE_HPP
