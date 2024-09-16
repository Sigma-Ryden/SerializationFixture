#ifndef SF_CORE_ARCHIVE_BASE_HPP
#define SF_CORE_ARCHIVE_BASE_HPP

#include <SerializationFixture/Core/ArchiveTraits.hpp>

#include <SerializationFixture/Detail/Meta.hpp>

namespace sf
{

namespace core
{

struct ioarchive_t
{
    ioarchive_t(::xxsf_archive_traits_key_type trait = ::xxsf_archive_traits_base_key, bool readonly = false)
        : trait(trait), readonly(readonly) {}

#ifdef SF_DEBUG
    virtual ~ioarchive_t() = default;
#endif // SF_DEBUG

    ::xxsf_archive_traits_key_type const trait;
    bool const readonly;
};

struct iarchive_common_t {};
struct oarchive_common_t {};

} // namespace core

namespace meta
{

template <class ArchiveType> struct is_iarchive : std::is_base_of<core::iarchive_common_t, ArchiveType> {};
template <class ArchiveType> struct is_oarchive : std::is_base_of<core::oarchive_common_t, ArchiveType> {};

template <class ArchiveType> struct is_ioarchive : one<is_iarchive<ArchiveType>, is_oarchive<ArchiveType>> {};

} // namespace meta

} // namespace sf

#endif // SF_CORE_ARCHIVE_BASE_HPP
