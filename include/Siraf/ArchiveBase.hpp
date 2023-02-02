#ifndef SIRAF_ARCHIVE_BASE_HPP
#define SIRAF_ARCHIVE_BASE_HPP

#include <Siraf/Utility.hpp>

#ifndef SIRAF_ARCHIVE_MAX_TRAIT_KEY
    #define SIRAF_ARCHIVE_MAX_TRAIT_KEY 4
#endif // SIRAF_ARCHIVE_MAX_TRAIT_KEY

#define SERIALIZATION_ARCHIVE(...)                                                                      \
    public:                                                                                             \
    virtual auto trait() const noexcept -> key_type override                                            \
    { return ::siraf::core::ArchiveTraitKey<__VA_ARGS__>::key; }

namespace siraf
{

namespace core
{

struct ArchiveCore
{
    using key_type = let::u8;

    static constexpr auto base_key = key_type(-1);
    static constexpr auto max_key = key_type(SIRAF_ARCHIVE_MAX_TRAIT_KEY);
};

template <class Archive> struct ArchiveTraitKey
{
    static constexpr auto key = ArchiveCore::base_key;
};

class ArchiveBase
{
public:
    using key_type = ArchiveCore::key_type;

protected:
    virtual ~ArchiveBase() = default;

public:
    virtual auto trait() const noexcept -> key_type
    {
        return ArchiveTraitKey<ArchiveBase>::key;
    }
};

} // namespace core

} // namespace siraf

#endif // SIRAF_ARCHIVE_BASE_HPP
