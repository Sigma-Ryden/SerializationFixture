#ifndef EIGHTSER_CORE_ARCHIVE_TRAITS_HPP
#define EIGHTSER_CORE_ARCHIVE_TRAITS_HPP

#include <cstdint> // uint8_t

#ifndef EIGHTSER_ARCHIVE_TRAIT_MAX_KEY_SIZE
    #define EIGHTSER_ARCHIVE_TRAIT_MAX_KEY_SIZE 4
#endif // EIGHTSER_ARCHIVE_TRAIT_MAX_KEY_SIZE

#define EXPORT_SERIALIZABLE_ARCHIVE(archive_key, archive_type, ...) \
    template <> struct xxeightser_archive_traits<__VA_ARGS__> { \
        static constexpr auto key = ::xxeightser_archive_traits_key_type(archive_key); \
    }; \
    template <> struct xxeightser_##archive_type##archive_registry<archive_key> { \
        using type = __VA_ARGS__; \
    };

namespace eightser
{

struct ioarchive_t;

} // namespace eightser

using xxeightser_archive_traits_key_type = std::uint8_t;
static constexpr auto xxeightser_archive_traits_base_key = ::xxeightser_archive_traits_key_type(EIGHTSER_ARCHIVE_TRAIT_MAX_KEY_SIZE);

template <class ArchiveType>
struct xxeightser_archive_traits
{
    static constexpr auto key = xxeightser_archive_traits_base_key;
};

template <::xxeightser_archive_traits_key_type ArchiveKey>
struct xxeightser_iarchive_registry { using type = ::eightser::ioarchive_t; };

template <::xxeightser_archive_traits_key_type ArchiveKey>
struct xxeightser_oarchive_registry { using type = ::eightser::ioarchive_t; };

#endif // EIGHTSER_CORE_ARCHIVE_TRAITS_HPP
