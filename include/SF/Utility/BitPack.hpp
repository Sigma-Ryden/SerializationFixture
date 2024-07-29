#ifndef SF_UTILITY_BIT_PACK_HPP
#define SF_UTILITY_BIT_PACK_HPP

#include <SF/Core/TypeCore.hpp>

#include <SF/OArchive.hpp>
#include <SF/IArchive.hpp>

#include <SF/Utility/Detail/Macro.hpp> // SF_BITPACK

#include <SF/Detail/MetaMacro.hpp>
#include <SF/Detail/Preprocessor.hpp> // SF_FIRST_ARGUMENT, SF_CONCAT

// Signature:
// BITPACK(common_fields_type, archive, (object.field0, field0_bits), (object.field1, field1_bits), ...)
// BITPACK macro will generate code:
// {
//     auto xxbitpack = ::sf::bitpack<common_fields_type>(archive);
//     object.field0 = xxbitpack(object.field0, field0_bits);
//     object.field1 = xxbitpack(object.field1, field1_bits);
//     and etc.
// }
#define BITPACK(common_fields_type, archive, ...) SF_BITPACK(common_fields_type, archive, __VA_ARGS__)

namespace sf
{

namespace detail
{

template <class Archive, typename T, typename enable = void>
struct bitpack_t;

template <class Archive, typename T>
struct bitpack_t<Archive, T, typename std::enable_if<sf::meta::is_oarchive<Archive>::value>::type>
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
struct bitpack_t<Archive, T, typename std::enable_if<sf::meta::is_iarchive<Archive>::value>::type>
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
