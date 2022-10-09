#ifndef SIFAR_POLYMORPHIC_ARCHIVE_HPP
#define SIFAR_POLYMORPHIC_ARCHIVE_HPP

#include <Sifar/ArchiveBase.hpp>
#include <Sifar/ArchiveTrait.hpp>

#include <Sifar/Access.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace dynamic
{

class PolymorphicArchive
{
public:
    using Archive  = core::ArchiveBase;
    using key_type = core::ArchiveCore::key_type;

    static constexpr key_type max_key = core::ArchiveCore::max_key;

public:
    template <class T> static void save(Archive& archive, T& object)
    {
        save_impl(archive, object);
    }

    template <class T> static void load(Archive& archive, T& object)
    {
       load_impl(archive, object);
    }

private:
    template <class Archive> static constexpr bool is_valid_archive() noexcept
    {
        return core::ArchiveTraitKey<Archive>::key != core::ArchiveCore::base_key;
    }

private:
    template <key_type Key, class T, SIFAR_REQUIRE(Key == max_key)>
    static void save_impl(Archive& archive, T& object)
    {
        throw "The write archive has invalid type key.";
    }

    template <key_type Key = 0, class T, SIFAR_REQUIRE(Key < max_key)>
    static void save_impl(Archive& archive, T& object)
    {
        using DerivedArchive = typename core::WriteArchiveTrait<Key>::type;

        if (core::ArchiveTraitKey<DerivedArchive>::key == archive.trait())
            return try_save<DerivedArchive>(archive, object);

        save_impl<Key + 1>(archive, object);
    }

    template <key_type Key, class T, SIFAR_REQUIRE(Key == max_key)>
    static void load_impl(Archive& archive, T& object)
    {
        throw "The read archive has invalid type key.";
    }

    template <key_type Key = 0, class T, SIFAR_REQUIRE(Key < max_key)>
    static void load_impl(Archive& archive, T& object)
    {
        using DerivedArchive = typename core::ReadArchiveTrait<Key>::type;

        if (core::ArchiveTraitKey<DerivedArchive>::key == archive.trait())
            return try_load<DerivedArchive>(archive, object);

        load_impl<Key + 1>(archive, object);
    }

    template <class WriteArchive, class T,
              SIFAR_REQUIRE(not is_valid_archive<WriteArchive>())>
    static void try_save(Archive& archive, T& object) { /*pass*/ }

    template <class WriteArchive, class T,
              SIFAR_REQUIRE(is_valid_archive<WriteArchive>())>
    static void try_save(Archive& archive, T& object)
    {
        auto write_archive = dynamic_cast<WriteArchive*>(&archive);
        if (write_archive == nullptr)
            throw "The write archive was not registered.";

        Access::save(*write_archive, object);
    }

    template <class ReadArchive, class T,
              SIFAR_REQUIRE(not is_valid_archive<ReadArchive>())>
    static void try_load(Archive& archive, T& object) { /*pass*/ }

    template <class ReadArchive, class T,
              SIFAR_REQUIRE(is_valid_archive<ReadArchive>())>
    static void try_load(Archive& archive, T& object)
    {
        auto read_archive = dynamic_cast<ReadArchive*>(&archive);
        if (read_archive == nullptr)
            throw "The read archive was not registered.";

        Access::load(*read_archive, object);
    }
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_POLYMORPHIC_ARCHIVE_HPP
