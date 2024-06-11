#ifndef SF_CORE_ARCHIVE_TRAITS_HPP
#define SF_CORE_ARCHIVE_TRAITS_HPP

#include <SF/Core/ArchiveBase.hpp>

#define EXPORT_SERIALIZATION_ARCHIVE(archive_key, archive_type, ...)                                    \
    namespace sf { namespace core {                                                                     \
        template <> struct archive_traits_key_t<__VA_ARGS__> {                                          \
            static constexpr ioarchive_t::key_type key = archive_key;                                   \
        };                                                                                              \
        template <> struct archive_type##archive_traits<archive_traits_key_t<__VA_ARGS__>::key> {       \
            using type = __VA_ARGS__;                                                                   \
        };                                                                                              \
    }}

namespace sf
{

namespace core
{

template <ioarchive_t::key_type I> struct iarchive_traits { using type = ioarchive_t; };
template <ioarchive_t::key_type I> struct oarchive_traits { using type = ioarchive_t; };

} // namespace core

} // namespace sf

#endif // SF_CORE_ARCHIVE_TRAITS_HPP
