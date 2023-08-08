#ifndef SF_UTILITY_BIT_PACK_HPP
#define SF_UTILITY_BIT_PACK_HPP

#include <SF/Core/TypeCore.hpp>

#include <SF/WriteArchive.hpp>
#include <SF/ReadArchive.hpp>

#include <SF/Utility/Detail/Macro.hpp> // _BITPACK_IMPLEMENTATION

#include <SF/Detail/MetaMacro.hpp>
#include <SF/Detail/Preprocessor.hpp> // SFFIRST_ARGUMENT, SFCONCAT

// Signature:
// BITPACK(common_fields_type)(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
#define BITPACK(...) _BITPACK_IMPLEMENTATION(__VA_ARGS__)

// Signature:
// FBITPACK(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
#define FBITPACK(...) BITPACK()(__VA_ARGS__)

// FBITPACK use fixed common_fields_type size version.
// BITPACK macro will generate code:
// {
//     auto __bitpack = ::sf::bitpack<common_fields_type>(archive);
//     object.field0 = __bitpack(object.field0, field0_bits);
//     object.field1 = __bitpack(object.field1, field1_bits);
//     and etc.
// }

namespace sf
{

namespace detail
{

template <class Archive, typename T, typename enable = void>
struct BitPack;

template <class Archive, typename T>
struct BitPack<Archive, T, SFWHEN(sf::meta::is_write_archive<Archive>::value)>
{
    Archive& archive;
    T data{};
    std::size_t offset{};

    BitPack(Archive& archive) : archive(archive) {}
    ~BitPack() { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        // same as data = data | (field << offset);
        data |= field << offset;
        offset += bits;

        return field;
    }
};

template <class Archive, typename T>
struct BitPack<Archive, T, SFWHEN(sf::meta::is_read_archive<Archive>::value)>
{
    Archive& archive;
    T data{};

    BitPack(Archive& archive) : archive(archive) { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        //same as field = data & ~(0xf...f << bits)
        field = data & ~(~T{} << bits);
        data >>= bits;

        return field;
    }
};

} // namespace detail

template <typename PackType = let::u32, class Archive>
detail::BitPack<Archive, PackType> bitpack(Archive& archive) noexcept { return { archive }; }

} // namespace sf

#endif // SF_UTILITY_BIT_PACK_HPP
