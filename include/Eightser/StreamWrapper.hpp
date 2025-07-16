#ifndef EIGHTSER_STREAM_WRAPPER_HPP
#define EIGHTSER_STREAM_WRAPPER_HPP

#include <cstddef> // size_t
#include <vector> // vector
#include <fstream> // ifstream, ofstream

#include <Eightser/Core/Memory.hpp>

#ifndef EIGHTSER_BYTE_STREAM_RESERVE_SIZE
    #define EIGHTSER_BYTE_STREAM_RESERVE_SIZE std::size_t(4096)
#endif // EIGHTSER_BYTE_STREAM_RESERVE_SIZE

namespace eightser
{

namespace wrapper
{

template <typename OutputStreamType = std::vector<unsigned char>>
class obyte_stream_t
{
protected:
    using item_type = typename OutputStreamType::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'eightser::wrapper::obyte_stream_t'.");

public:
    OutputStreamType& storage;

public:
    obyte_stream_t(OutputStreamType& stream) noexcept : storage(stream)
    {
        storage.reserve(EIGHTSER_BYTE_STREAM_RESERVE_SIZE); // default reserve memory
    }

    template <typename CharType>
    void call(CharType const* data, std::size_t size)
    {
        auto it = memory::const_byte_cast<item_type>(data);
        while (size > 0)
        {
            storage.emplace_back(*it++);
            --size;
        }
    }
};

template <typename InputStreamType = std::vector<unsigned char>>
struct ibyte_stream_t
{
protected:
    using item_type = typename InputStreamType::value_type;

protected:
    static_assert(sizeof(item_type) == 1, "Require byte size for 'eightser::wrapper::ibyte_stream_t'.");

public:
    InputStreamType& storage;
    std::size_t offset;

    ibyte_stream_t(InputStreamType& stream) noexcept : storage(stream), offset() {}

    template <typename CharType>
    void call(CharType* data, std::size_t size)
    {
        auto it = memory::byte_cast<item_type>(data);
        while (size > 0)
        {
            *it++ = storage[offset++];
            --size;
        }
    }
};

template <typename OutputStreamType = std::ofstream>
class ofile_stream_t
{
public:
    OutputStreamType& file;

public:
    ofile_stream_t(OutputStreamType& stream) noexcept : file(stream) {}

    template <typename CharType>
    void call(CharType const* data, std::size_t memory_size)
    {
        file.write(memory::const_byte_cast(data), memory_size);
    }
};

template <typename InputStreamType = std::ifstream>
class ifile_stream_t
{
public:
    InputStreamType& file;

public:
    ifile_stream_t(InputStreamType& stream) noexcept : file(stream) {}

    template <typename CharType>
    void call(CharType* data, std::size_t memory_size)
    {
        file.read(memory::byte_cast(data), memory_size);
    }
};

} // namespace wrapper

} // namespace eightser

#endif // EIGHTSER_STREAM_WRAPPER_HPP
