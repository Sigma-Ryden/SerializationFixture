#ifndef SIFAR_ARCHIVE_BASE_HPP
#define SIFAR_ARCHIVE_BASE_HPP

#include <cstdint> // int8_t

#include <Sifar/Utility.hpp>

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
    static constexpr auto max_key = key_type(8);
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
