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
    std::vector<unsigned char>& file;

public:
    OByteStream(std::vector<unsigned char>& stream) : file(stream)
    {
        file.reserve(1024);
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = memory::const_byte_cast(data);
        while (size > 0)
        {
            file.emplace_back(static_cast<unsigned char>(*it++));
            --size;
        }
    }
};

struct IByteStream
{
public:
    std::vector<unsigned char>& file;
    std::size_t offset;

    IByteStream(std::vector<unsigned char>& data) : file(data), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = memory::byte_cast(data);
        while (size > 0)
        {
            *it++ = static_cast<unsigned char>(file[offset++]);
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
