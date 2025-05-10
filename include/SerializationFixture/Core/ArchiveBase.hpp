#ifndef SF_CORE_ARCHIVE_BASE_HPP
#define SF_CORE_ARCHIVE_BASE_HPP

#include <SerializationFixture/Core/ArchiveTraits.hpp>

#include <SerializationFixture/Detail/Meta.hpp>

using xxsf_archive_type_key_type = std::uint8_t;

namespace sf
{

constexpr xxsf_archive_type_key_type bin = 0;

#ifdef SF_EXTENDED_SERIALIZATION
constexpr xxsf_archive_type_key_type xml = 1;
constexpr xxsf_archive_type_key_type json = 2;
#endif // SF_EXTENDED_SERIALIZATION

struct ioarchive_t
{
    ioarchive_t(::xxsf_archive_traits_key_type trait, ::xxsf_archive_type_key_type type, bool readonly)
        : trait(trait), type(type), readonly(readonly) {}

#ifdef SF_DEBUG
    virtual ~ioarchive_t() = default;
#endif // SF_DEBUG

    std::uint64_t trait : sizeof(xxsf_archive_traits_key_type) * 8;
    std::uint64_t type : sizeof(xxsf_archive_type_key_type) * 8;
    std::uint64_t readonly : 1;
};

namespace meta
{

template <class ArchiveType> struct is_iarchive : std::false_type {};
template <class ArchiveType> struct is_oarchive : std::false_type {};
template <class ArchiveType> struct is_ioarchive : std::is_base_of<ioarchive_t, ArchiveType> {};

} // namespace meta

} // namespace sf

#endif // SF_CORE_ARCHIVE_BASE_HPP
