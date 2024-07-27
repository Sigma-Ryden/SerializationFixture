#ifndef SF_UTILITY_BIT_PACK_HPP
#define SF_UTILITY_BIT_PACK_HPP

#include <SF/Core/TypeCore.hpp>

#include <SF/OArchive.hpp>
#include <SF/IArchive.hpp>

#include <SF/Utility/Detail/Macro.hpp> // _BITPACK_IMPLEMENTATION

#include <SF/Detail/MetaMacro.hpp>
#include <SF/Detail/Preprocessor.hpp> // SF_FIRST_ARGUMENT, SF_CONCAT

// Signature:
// BITPACK(common_fields_type)(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
#define BITPACK(...) _BITPACK_IMPLEMENTATION(__VA_ARGS__)

// Signature:
// FBITPACK(archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
#define FBITPACK(...) BITPACK()(__VA_ARGS__)

// FBITPACK use fixed common_fields_type size version.
// BITPACK macro will generate code:
// {
//     auto xxbitpack = ::sf::bitpack<common_fields_type>(archive);
//     object.field0 = xxbitpack(object.field0, field0_bits);
//     object.field1 = xxbitpack(object.field1, field1_bits);
//     and etc.
// }

namespace sf
{

namespace detail
{

template <class Archive, typename T, typename enable = void>
struct bitpack_t;

template <class Archive, typename T>
struct bitpack_t<Archive, T, SF_WHEN(sf::meta::is_oarchive<Archive>::value)>
{
    Archive& archive;
    T data{};
    std::size_t offset{};

    bitpack_t(Archive& archive) : archive(archive) {}
    ~bitpack_t() { archive & data; }

    T operator()(T field, std::size_t bits) noexcept
    {
        // same as data = data | (field << offset);
        data |= field << offset;
        offset += bits;

        return field;
    }
};

template <class Archive, typename T>
struct bitpack_t<Archive, T, SF_WHEN(sf::meta::is_iarchive<Archive>::value)>
{
    Archive& archive;
    T data{};

    bitpack_t(Archive& archive) : archive(archive) { archive & data; }

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
detail::bitpack_t<Archive, PackType> bitpack(Archive& archive) noexcept { return { archive }; }

} // namespace sf

#endif // SF_UTILITY_BIT_PACK_HPP
