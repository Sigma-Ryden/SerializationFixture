#ifndef SF_CORE_ARCHIVE_BASE_HPP
#define SF_CORE_ARCHIVE_BASE_HPP

#include <SF/Core/ArchiveTraits.hpp>

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

    const ::xxsf_archive_traits_key_type trait;
    const bool readonly;
};

} // namespace core

} // namespace sf

#endif // SF_CORE_ARCHIVE_BASE_HPP
