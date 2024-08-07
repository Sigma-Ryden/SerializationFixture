#ifndef SF_CORE_ARCHIVE_TRAITS_HPP
#define SF_CORE_ARCHIVE_TRAITS_HPP

#include <SF/Core/TypeCore.hpp>

#ifndef SF_ARCHIVE_TRAIT_MAX_KEY_SIZE
    #define SF_ARCHIVE_TRAIT_MAX_KEY_SIZE 3
#endif // SF_ARCHIVE_TRAIT_MAX_KEY_SIZE

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                \
    template <> struct xxsf_archive_traits<__VA_ARGS__> {                                           \
        static constexpr auto key = ::xxsf_archive_traits_key_type(archive_key);                    \
    };                                                                                              \
    template <> struct xxsf_##archive_type##archive_registry<archive_key> {                         \
        using type = __VA_ARGS__;                                                                   \
    };                                                                                              \

namespace sf
{

namespace core
{

struct ioarchive_t;

} // namespace core

} // namespace sf

using xxsf_archive_traits_key_type = ::sf::let::u8;
static constexpr auto xxsf_archive_traits_base_key = xxsf_archive_traits_key_type(-1);

template <class ArchiveType>
struct xxsf_archive_traits
{
    static constexpr auto key = xxsf_archive_traits_base_key;
};

template <xxsf_archive_traits_key_type ArchiveKey>
struct xxsf_iarchive_registry { using type = ::sf::core::ioarchive_t; };

template <xxsf_archive_traits_key_type ArchiveKey>
struct xxsf_oarchive_registry { using type = ::sf::core::ioarchive_t; };

#endif // SF_CORE_ARCHIVE_TRAITS_HPP
