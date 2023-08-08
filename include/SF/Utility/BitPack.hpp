#ifndef SF_BIT_PACK_HPP
#define SF_BIT_PACK_HPP

#include <SF/Core/TypeCore.hpp>

#include <SF/WriteArchive.hpp>
#include <SF/ReadArchive.hpp>

#include <SF/Detail/MetaMacro.hpp>
#include <SF/Detail/Preprocessor.hpp> // SFFIRST_ARGUMENT, SFCONCAT

#define _BITPACK_N(...) SFCONCAT(_BITPACK_, SFVA_ARGS_SIZE(__VA_ARGS__))(__VA_ARGS__)
#define _BITPACK_IMPLEMENTATION(archive, ...) archive); _BITPACK_N(__VA_ARGS__) }

// BITPACK macro signature:
// BITPACK(common_fields_type)(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
// will generate code:
// {
//     auto __bitpack = ::sf::bitpack<common_fields_type>(archive);
//     object.field0 = __bitpack(object.field0, field0_bits);
//     object.field1 = __bitpack(object.field1, field1_bits);
//     ...
// }
#define BITPACK(...) { auto __bitpack = ::sf::bitpack<__VA_ARGS__>( _BITPACK_IMPLEMENTATION

#define _BITFIELD(field_and_size) SFFIRST_ARGUMENT field_and_size = __bitpack field_and_size;

#define _BITPACK_1(field_and_size) _BITFIELD(field_and_size)
#define _BITPACK_2(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_1(__VA_ARGS__)
#define _BITPACK_3(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_2(__VA_ARGS__)
#define _BITPACK_4(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_3(__VA_ARGS__)
#define _BITPACK_5(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_4(__VA_ARGS__)
#define _BITPACK_6(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_5(__VA_ARGS__)
#define _BITPACK_7(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_6(__VA_ARGS__)
#define _BITPACK_8(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_7(__VA_ARGS__)

#define _BITPACK_9(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_8(__VA_ARGS__)
#define _BITPACK_10(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_9(__VA_ARGS__)
#define _BITPACK_11(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_10(__VA_ARGS__)
#define _BITPACK_12(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_11(__VA_ARGS__)
#define _BITPACK_13(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_12(__VA_ARGS__)
#define _BITPACK_14(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_13(__VA_ARGS__)
#define _BITPACK_15(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_14(__VA_ARGS__)
#define _BITPACK_16(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_15(__VA_ARGS__)

#define _BITPACK_17(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_16(__VA_ARGS__)
#define _BITPACK_18(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_17(__VA_ARGS__)
#define _BITPACK_19(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_18(__VA_ARGS__)
#define _BITPACK_20(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_19(__VA_ARGS__)
#define _BITPACK_21(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_20(__VA_ARGS__)
#define _BITPACK_22(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_21(__VA_ARGS__)
#define _BITPACK_23(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_22(__VA_ARGS__)
#define _BITPACK_24(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_23(__VA_ARGS__)

#define _BITPACK_25(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_24(__VA_ARGS__)
#define _BITPACK_26(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_25(__VA_ARGS__)
#define _BITPACK_27(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_26(__VA_ARGS__)
#define _BITPACK_28(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_27(__VA_ARGS__)
#define _BITPACK_29(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_28(__VA_ARGS__)
#define _BITPACK_30(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_29(__VA_ARGS__)
#define _BITPACK_31(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_30(__VA_ARGS__)
#define _BITPACK_32(field_and_size, ...) _BITFIELD(field_and_size) _BITPACK_31(__VA_ARGS__)

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

#endif // SF_BIT_PACK_HPP
