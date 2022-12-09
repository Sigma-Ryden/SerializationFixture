#ifndef SIFAR_ARCHIVE_BASE_HPP
#define SIFAR_ARCHIVE_BASE_HPP

#include <Sifar/Utility.hpp>

#ifndef SIFAR_ARCHIVE_MAX_TRAIT_KEY
    #define SIFAR_ARCHIVE_MAX_TRAIT_KEY 2
#endif // SIFAR_ARCHIVE_MAX_TRAIT_KEY

#define SERIALIZATION_ARCHIVE(...)                                                                      \
    public:                                                                                             \
    virtual auto trait() const noexcept -> key_type override                                            \
    { return ::sifar::core::ArchiveTraitKey<__VA_ARGS__>::key; }

namespace sifar
{

namespace core
{

struct ArchiveCore
{
    using key_type = let::u8;

    static constexpr auto base_key = key_type(-1);
    static constexpr auto max_key = key_type(SIFAR_ARCHIVE_MAX_TRAIT_KEY);
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

} // namespace sifar

#endif // SIFAR_ARCHIVE_BASE_HPP
