#ifndef SIRAF_CORE_ARCHIVE_TRAIT_HPP
#define SIRAF_CORE_ARCHIVE_TRAIT_HPP

#include <Siraf/Core/ArchiveBase.hpp>

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                    \
    namespace siraf { namespace core {                                                                  \
        template <> struct ArchiveTraitKey<archive_type<__VA_ARGS__>> {                                 \
            static constexpr ArchiveBase::key_type key = archive_key;                                   \
        };                                                                                              \
        template <> struct archive_type##Trait<ArchiveTraitKey<archive_type<__VA_ARGS__>>::key> {       \
            using type = archive_type<__VA_ARGS__>;                                                     \
        };                                                                                              \
    }}

namespace siraf
{

namespace core
{

template <ArchiveBase::key_type I> struct ReadArchiveTrait { using type = ArchiveBase; };
template <ArchiveBase::key_type I> struct WriteArchiveTrait { using type = ArchiveBase; };

} // namespace core

} // namespace siraf

#endif // SIRAF_CORE_ARCHIVE_TRAIT_HPP
