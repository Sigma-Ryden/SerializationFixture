#ifndef SF_CORE_ARCHIVE_BASE_HPP
#define SF_CORE_ARCHIVE_BASE_HPP

#include <SF/Core/TypeCore.hpp>

#ifndef SF_ARCHIVE_TRAIT_MAX_KEY_SIZE
    #define SF_ARCHIVE_TRAIT_MAX_KEY_SIZE 4
#endif // SF_ARCHIVE_MAX_TRAIT_KEY

namespace sf
{

namespace core
{

struct archive_traits
{
    using key_type = let::u8;

    static constexpr auto base_key = key_type(-1);
    static constexpr auto max_key = key_type(SF_ARCHIVE_TRAIT_MAX_KEY_SIZE);
};

template <class Archive> struct archive_traits_key_t
{
    static constexpr auto key = archive_traits::base_key;
};

class ioarchive_t
{
public:
    using key_type = archive_traits::key_type;

public:
    ioarchive_t(key_type trait, bool readonly) : trait(trait), readonly(readonly) {}

protected:
    virtual ~ioarchive_t() = default;

public:
    const key_type trait = archive_traits_key_t<ioarchive_t>::key;
    const bool readonly = false;
};
// ~TODO: temp impl
} // namespace core

} // namespace sf

#endif // SF_CORE_ARCHIVE_BASE_HPP
