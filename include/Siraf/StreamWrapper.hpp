#ifndef SIRAF_STREAM_WRAPPER_HPP
#define SIRAF_STREAM_WRAPPER_HPP

#include <cstddef> // size_t
#include <vector> // vector

#include <Siraf/Memory/Memory.hpp>

#ifndef SIRAF_BYTE_STREAM_RESERVE_SIZE
    #define SIRAF_BYTE_STREAM_RESERVE_SIZE std::size_t(1024)
#endif // SIRAF_BYTE_STREAM_RESERVE_SIZE

namespace siraf
{

namespace wrapper
{

using ByteContainer = std::vector<unsigned char>;

template <typename OutStream = ByteContainer>
class OByteStream
{
protected:
    using item_type = typename OutStream::value_type;

public:
    OutStream& storage;

public:
    OByteStream(OutStream& stream) : storage(stream)
    {
        storage.clear();
        storage.reserve(SIRAF_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = memory::const_byte_cast<item_type>(data);
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

public:
    InStream& storage;
    std::size_t offset;

    IByteStream(InStream& stream) : storage(stream), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = memory::byte_cast<item_type>(data);
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
        file.write(memory::const_byte_cast(data), memory_size);
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
        file.read(memory::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace siraf

#endif // SIRAF_STREAM_WRAPPER_HPP
