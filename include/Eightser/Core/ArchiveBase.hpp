#ifndef EIGHTSER_CORE_ARCHIVE_BASE_HPP
#define EIGHTSER_CORE_ARCHIVE_BASE_HPP

#include <Eightser/Core/ArchiveTraits.hpp>

#include <Eightser/Detail/Meta.hpp>

using xxeightser_archive_type_key_type = std::uint8_t;

namespace eightser
{

constexpr xxeightser_archive_type_key_type bin = 0;

#ifdef EIGHTSER_EXTENDED_SERIALIZATION
constexpr xxeightser_archive_type_key_type xml = 1;
constexpr xxeightser_archive_type_key_type json = 2;
#endif // EIGHTSER_EXTENDED_SERIALIZATION

struct ioarchive_t
{
    ioarchive_t(::xxeightser_archive_traits_key_type trait, ::xxeightser_archive_type_key_type type, bool readonly)
        : trait(trait), type(type), readonly(readonly) {}

#ifdef EIGHTSER_DEBUG
    virtual ~ioarchive_t() = default;
#endif // EIGHTSER_DEBUG

    std::uint64_t trait : sizeof(xxeightser_archive_traits_key_type) * 8;
    std::uint64_t type : sizeof(xxeightser_archive_type_key_type) * 8;
    std::uint64_t readonly : 1;
};

namespace meta
{

template <class ArchiveType> struct is_iarchive : std::false_type {};
template <class ArchiveType> struct is_oarchive : std::false_type {};
template <class ArchiveType> struct is_ioarchive : std::is_base_of<ioarchive_t, ArchiveType> {};

} // namespace meta

} // namespace eightser

#endif // EIGHTSER_CORE_ARCHIVE_BASE_HPP
