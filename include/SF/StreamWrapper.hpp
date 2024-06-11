#ifndef SF_STREAM_WRAPPER_HPP
#define SF_STREAM_WRAPPER_HPP

#include <cstddef> // size_t
#include <vector> // vector

#include <SF/Core/Memory.hpp>

#ifndef SF_BYTE_STREAM_RESERVE_SIZE
    #define SF_BYTE_STREAM_RESERVE_SIZE std::size_t(4096)
#endif // SF_BYTE_STREAM_RESERVE_SIZE

namespace sf
{

namespace wrapper
{

using byte_container_t = std::vector<unsigned char>;

template <typename OutStream = byte_container_t>
class obyte_stream_t
{
protected:
    using item_type = typename OutStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::obyte_stream_t'.");

public:
    OutStream& storage;

public:
    obyte_stream_t(OutStream& stream) noexcept : storage(stream)
    {
        storage.reserve(SF_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename T>
    void call(const T* data, std::size_t size)
    {
        auto it = memory_t::const_byte_cast<item_type>(data);
        while (size > 0)
        {
            storage.emplace_back(*it++);
            --size;
        }
    }
};

template <typename InStream = byte_container_t>
struct ibyte_stream_t
{
protected:
    using item_type = typename InStream::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'sf::wrapper::ibyte_stream_t'.");

public:
    InStream& storage;
    std::size_t offset;

    ibyte_stream_t(InStream& stream) noexcept : storage(stream), offset() {}

    template <typename T>
    void call(T* data, std::size_t size)
    {
        auto it = memory_t::byte_cast<item_type>(data);
        while (size > 0)
        {
            *it++ = storage[offset++];
            --size;
        }
    }
};

template <typename OutStream>
class ofile_stream_t
{
public:
    OutStream& file;

public:
    ofile_stream_t(OutStream& stream) noexcept : file(stream) {}

    template <typename T>
    void call(const T* data, std::size_t memory_size)
    {
        file.write(memory_t::const_byte_cast(data), memory_size);
    }
};

template <typename InStream>
class ifile_stream_t
{
public:
    InStream& file;

public:
    ifile_stream_t(InStream& stream) noexcept : file(stream) {}

    template <typename T>
    void call(T* data, std::size_t memory_size)
    {
        file.read(memory_t::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace sf

#endif // SF_STREAM_WRAPPER_HPP
