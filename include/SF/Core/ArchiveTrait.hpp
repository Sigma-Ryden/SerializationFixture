#ifndef SF_CORE_ARCHIVE_TRAIT_HPP
#define SF_CORE_ARCHIVE_TRAIT_HPP

#include <SF/Core/ArchiveBase.hpp>

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                    \
    namespace sf { namespace core {                                                                     \
        template <> struct ArchiveTraitKey<__VA_ARGS__> {                                               \
            static constexpr IOArchive::key_type key = archive_key;                                     \
        };                                                                                              \
        template <> struct archive_type##ArchiveTrait<ArchiveTraitKey<__VA_ARGS__>::key> {              \
            using type = __VA_ARGS__;                                                                   \
        };                                                                                              \
    }}

namespace sf
{

namespace core
{

template <IOArchive::key_type I> struct IArchiveTrait { using type = IOArchive; };
template <IOArchive::key_type I> struct OArchiveTrait { using type = IOArchive; };

} // namespace core

} // namespace sf

#endif // SF_CORE_ARCHIVE_TRAIT_HPP
