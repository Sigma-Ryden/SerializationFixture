#ifndef SIFAR_ARCHIVE_TRAIT_HPP
#define SIFAR_ARCHIVE_TRAIT_HPP

#include <cstddef> // size_t

#include <Sifar/ArchiveBase.hpp>

#include <Sifar/Utility.hpp>

#define _EXPORT_SERIALIZATION_ARCHIVE_TYPE_TRAIT(archive_type, ...)                                     \
    namespace sifar { namespace core {                                                                  \
        template <> struct archive_type##Trait<ArchiveTraitKey<archive_type<__VA_ARGS__>>::key>         \
        { using type = archive_type<__VA_ARGS__>; };                                                    \
    }}

namespace sifar
{

namespace core
{

template <class Archive> struct ArchiveTrait
{
    static constexpr let::u8 key = 0;
};

template <ArchiveCore::key_type I> struct ReadArchiveTrait { using type = ArchiveBase; };
template <ArchiveCore::key_type I> struct WriteArchiveTrait { using type = ArchiveBase; };

} // namespace core

} // namespace sifar

#endif // SIFAR_ARCHIVE_TRAIT_HPP
