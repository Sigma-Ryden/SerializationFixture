#ifndef SF_CORE_POLYMORPHIC_ARCHIVE_HPP
#define SF_CORE_POLYMORPHIC_ARCHIVE_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/ArchiveTrait.hpp>

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Core/Memory.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace core
{

class PolymorphicArchive
{
public:
    using Archive  = core::IOArchive;
    using key_type = core::IOArchive::key_type;

    static constexpr key_type max_key = core::ArchiveTrait::max_key;

public:
    template <class T> static void save(Archive& archive, T& data)
    {
        call<core::OArchiveTrait>(archive, data);
    }

    template <class T> static void load(Archive& archive, T& data)
    {
        call<core::IArchiveTrait>(archive, data);
    }

private:
    template <class Archive> struct is_valid_archive
        : meta::boolean<core::ArchiveTraitKey<Archive>::key != core::ArchiveTrait::base_key> {};

private:
    template <template <key_type> class ArchiveTrait,
              key_type Key, class T, SFREQUIRE(Key == max_key)>
    static void call(Archive& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <key_type> class ArchiveTrait,
              key_type Key = 0, class T, SFREQUIRE(Key < max_key)>
    static void call(Archive& archive, T& data)
    {
        using DerivedArchive = typename ArchiveTrait<Key>::type;

        if (core::ArchiveTraitKey<DerivedArchive>::key == archive.trait())
            return try_call<DerivedArchive>(archive, data);

        call<ArchiveTrait, Key + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(not is_valid_archive<DerivedArchive>::value)>
    static void try_call(Archive& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(is_valid_archive<DerivedArchive>::value)>
    static void try_call(Archive& archive, T& data)
    {
        auto derived_archive = dynamic_cast<DerivedArchive*>(&archive);

    #ifdef SF_DEBUG
        if (derived_archive == nullptr)
            throw "The read/write archive was registered incorrect.";
    #endif // SF_DEBUG

        proccess(*derived_archive, data);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::all<meta::is_oarchive<DerivedArchive>,
                                  ::Serialization::has_save_mode<T>>::value)>
    static void proccess(DerivedArchive& archive, T& object)
    {
        ::Serialization::save(archive, object);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::all<meta::is_iarchive<DerivedArchive>,
                                  ::Serialization::has_load_mode<T>>::value)>
    static void proccess(DerivedArchive& archive, T& object)
    {
        ::Serialization::load(archive, object);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::all<meta::is_archive<DerivedArchive>,
                                  meta::negation<::Serialization::has_save_mode<T>>,
                                  meta::negation<::Serialization::has_load_mode<T>>>::value)>
    static void proccess(DerivedArchive& archive, T& data)
    {
        process_data(archive, data);
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(meta::is_registered<T>::value)>
    static void process_data(DerivedArchive& archive, T& data)
    {
        archive & data;
    }

    template <class DerivedArchive, class T,
              SFREQUIRE(not meta::is_registered<T>::value)>
    static void process_data(DerivedArchive& archive, T& data)
    {
        throw "The 'T' type is unregistered.";
    }
};

template <class Archive, typename T,
          SFREQUIRE(meta::is_archive<Archive>::value)>
Archive& operator<< (Archive& archive, T&& data)
{
    PolymorphicArchive::save(archive, data);
    return archive;
}

template <class Archive, typename T,
          SFREQUIRE(meta::is_archive<Archive>::value)>
Archive& operator>> (Archive& archive, T&& data)
{
    PolymorphicArchive::load(archive, data);
    return archive;
}

} // namespace core

} // namespace sf

#endif // SF_CORE_POLYMORPHIC_ARCHIVE_HPP
