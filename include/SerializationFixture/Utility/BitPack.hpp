#ifndef SF_UTILITY_BIT_PACK_HPP
#define SF_UTILITY_BIT_PACK_HPP

#include <cstdint> // uint32_t

#include <SerializationFixture/OArchive.hpp>
#include <SerializationFixture/IArchive.hpp>

#include <SerializationFixture/Utility/Detail/Macro.hpp> // SF_BITPACK

#include <SerializationFixture/Detail/MetaMacro.hpp>
#include <SerializationFixture/Detail/Preprocessor.hpp> // SF_FIRST_ARGUMENT, SF_CONCAT

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

template <class ArchiveType, typename FieldType, typename enable = void>
struct bitpack_t;

template <class ArchiveType, typename FieldType>
struct bitpack_t<ArchiveType, FieldType, typename std::enable_if<sf::meta::is_oarchive<ArchiveType>::value>::type>
{
    ArchiveType& archive;
    FieldType data{};
    std::size_t offset{};

    bitpack_t(ArchiveType& archive) : archive(archive) {}
    ~bitpack_t() { archive & data; }

    FieldType operator()(FieldType field, std::size_t bits) noexcept
    {
        // same as data = data | (field << offset);
        data |= field << offset;
        offset += bits;

        return field;
    }
};

template <class ArchiveType, typename FieldType>
struct bitpack_t<ArchiveType, FieldType, typename std::enable_if<sf::meta::is_iarchive<ArchiveType>::value>::type>
{
    ArchiveType& archive;
    FieldType data{};

    bitpack_t(ArchiveType& archive) : archive(archive) { archive & data; }

    FieldType operator()(FieldType field, std::size_t bits) noexcept
    {
        //same as field = data & ~(0xf...f << bits)
        field = data & ~(~FieldType{} << bits);
        data >>= bits;

        return field;
    }
};

} // namespace detail

template <typename PackType = std::uint32_t, class ArchiveType>
detail::bitpack_t<ArchiveType, PackType> bitpack(ArchiveType& archive) noexcept { return { archive }; }

} // namespace sf

#endif // SF_UTILITY_BIT_PACK_HPP
