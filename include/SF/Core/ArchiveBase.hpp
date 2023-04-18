#ifndef SF_CORE_ARCHIVE_BASE_HPP
#define SF_CORE_ARCHIVE_BASE_HPP

#include <SF/Core/TypeCore.hpp>

#ifndef SF_ARCHIVE_MAX_TRAIT_KEY
    #define SF_ARCHIVE_MAX_TRAIT_KEY 4
#endif // SF_ARCHIVE_MAX_TRAIT_KEY

#define SERIALIZATION_ARCHIVE(...)                                                                      \
    virtual key_type trait() const noexcept override {                                                  \
        return ::sf::core::ArchiveTraitKey<__VA_ARGS__>::key;                                           \
    }

namespace sf
{

namespace core
{

struct ArchiveTraitBase
{
    using key_type = let::u8;

    static constexpr auto base_key = key_type(-1);
    static constexpr auto max_key = key_type(SF_ARCHIVE_MAX_TRAIT_KEY);
};

template <class Archive> struct ArchiveTraitKey
{
    static constexpr auto key = ArchiveTraitBase::base_key;
};

class ArchiveBase
{
public:
    using key_type = ArchiveTraitBase::key_type;

protected:
    virtual ~ArchiveBase() = default;

public:
    virtual auto trait() const noexcept -> key_type
    {
        return ArchiveTraitKey<ArchiveBase>::key;
    }
};


template <class ArchiveType>
struct Archive : ArchiveBase, ArchiveType {};

} // namespace core

} // namespace sf

#endif // SF_CORE_ARCHIVE_BASE_HPP
