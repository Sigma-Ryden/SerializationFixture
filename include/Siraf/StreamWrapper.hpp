#ifndef SIRAF_STREAM_WRAPPER_HPP
#define SIRAF_STREAM_WRAPPER_HPP

#include <cstddef> // size_t
#include <vector> // vector

#include <Siraf/Memory/Memory.hpp>

namespace siraf
{

namespace wrapper
{

class OByteStream
{
public:
    std::vector<unsigned char>& storage;

public:
    OByteStream(std::vector<unsigned char>& stream) : storage(stream)
    {
        storage.reserve(1024);
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = memory::const_byte_cast(data);
        while (size > 0)
        {
            storage.emplace_back(static_cast<unsigned char>(*it++));
            --size;
        }
    }
};

struct IByteStream
{
public:
    std::vector<unsigned char>& storage;
    std::size_t offset;

    IByteStream(std::vector<unsigned char>& data) : storage(data), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = memory::byte_cast(data);
        while (size > 0)
        {
            *it++ = static_cast<unsigned char>(storage[offset++]);
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
