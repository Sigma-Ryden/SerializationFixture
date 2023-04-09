#ifndef SF_STREAM_WRAPPER_HPP
#define SF_STREAM_WRAPPER_HPP

#include <cstddef> // size_t
#include <vector> // vector

#include <SF/Core/Memory.hpp>

#ifndef SF_BYTE_STREAM_RESERVE_SIZE
    #define SF_BYTE_STREAM_RESERVE_SIZE std::size_t(1024)
#endif // SF_BYTE_STREAM_RESERVE_SIZE

namespace sf
{

namespace wrapper
{

using ByteContainer = std::vector<unsigned char>;

template <typename OutStream = ByteContainer>
class OByteStream
{
protected:
    using item_type = typename OutStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::OByteStream'.");

public:
    OutStream& storage;

public:
    OByteStream(OutStream& stream) : storage(stream)
    {
        storage.reserve(SF_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = Memory::const_byte_cast<item_type>(data);
        while (size > 0)
        {
            storage.emplace_back(*it++);
            --size;
        }
    }
};

template <typename InStream = ByteContainer>
struct IByteStream
{
protected:
    using item_type = typename InStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::IByteStream'.");

public:
    InStream& storage;
    std::size_t offset;

    IByteStream(InStream& stream) : storage(stream), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = Memory::byte_cast<item_type>(data);
        while (size > 0)
        {
            *it++ = storage[offset++];
            --size;
        }
    }
};

template <typename OutStream>
class OFileStream
{
public:
    OutStream& file;

public:
    OFileStream(OutStream& stream) : file(stream) {}

    template <typename T>
    void call(const T* data, std::size_t memory_size)
    {
        file.write(Memory::const_byte_cast(data), memory_size);
    }
};

template <typename InStream>
class IFileStream
{
public:
    InStream& file;

public:
    IFileStream(InStream& stream) : file(stream) {}

    template <typename T>
    void call(T* data, std::size_t memory_size)
    {
        file.read(Memory::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace sf

#endif // SF_STREAM_WRAPPER_HPP
