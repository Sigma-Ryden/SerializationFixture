#ifndef SF_CORE_POLYMORPHIC_ARCHIVE_HPP
#define SF_CORE_POLYMORPHIC_ARCHIVE_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/ArchiveTraits.hpp>

#include <SF/Core/Serialization.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Core/Memory.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace core
{

class polymorphic_archive_t
{
public:
    using Archive  = ioarchive_t;
    using key_type = ioarchive_t::key_type;

    static constexpr key_type max_key = archive_traits::max_key;

public:
    template <class T> static void save(Archive& archive, T& data)
    {
        call<oarchive_traits>(archive, data);
    }

    template <class T> static void load(Archive& archive, T& data)
    {
        call<iarchive_traits>(archive, data);
    }

private:
    template <class Archive> struct is_valid_archive
        : std::integral_constant<bool, archive_traits_key_t<Archive>::key != archive_traits::base_key> {};

private:
    template <template <key_type> class archive_traits,
              key_type Key, class T, SF_REQUIRE(Key == max_key)>
    static void call(Archive& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <key_type> class archive_traits,
              key_type Key = 0, class T, SF_REQUIRE(Key < max_key)>
    static void call(Archive& archive, T& data)
    {
        using DerivedArchive = typename archive_traits<Key>::type;

        if (archive_traits_key_t<DerivedArchive>::key == archive.trait())
            return try_call<DerivedArchive>(archive, data);

        call<archive_traits, Key + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(not is_valid_archive<DerivedArchive>::value)>
    static void try_call(Archive& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(is_valid_archive<DerivedArchive>::value)>
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
              SF_REQUIRE(meta::all<meta::is_oarchive<DerivedArchive>,
                                   ::xxsf::has_save_mode<T>>::value)>
    static void proccess(DerivedArchive& archive, T& object)
    {
        ::xxsf::save(archive, object);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(meta::all<meta::is_iarchive<DerivedArchive>,
                                   ::xxsf::has_load_mode<T>>::value)>
    static void proccess(DerivedArchive& archive, T& object)
    {
        ::xxsf::load(archive, object);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(meta::all<meta::is_ioarchive<DerivedArchive>,
                                   meta::negation<::xxsf::has_save_mode<T>>,
                                   meta::negation<::xxsf::has_load_mode<T>>>::value)>
    static void proccess(DerivedArchive& archive, T& data)
    {
        process_data(archive, data);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(meta::is_registered_extern<T>::value)>
    static void process_data(DerivedArchive& archive, T& data)
    {
        archive & data;
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(not meta::is_registered_extern<T>::value)>
    static void process_data(DerivedArchive& archive, T& data)
    {
        throw "The 'T' type is unregistered.";
    }
};

template <class Archive, typename T,
          SF_REQUIRE(meta::is_archive<Archive>::value)>
Archive& operator<< (Archive& archive, T&& data)
{
    polymorphic_archive_t::save(archive, data);
    return archive;
}

template <class Archive, typename T,
          SF_REQUIRE(meta::is_archive<Archive>::value)>
Archive& operator>> (Archive& archive, T&& data)
{
    polymorphic_archive_t::load(archive, data);
    return archive;
}

} // namespace core

} // namespace sf

#endif // SF_CORE_POLYMORPHIC_ARCHIVE_HPP
