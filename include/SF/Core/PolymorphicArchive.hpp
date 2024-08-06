#ifndef SF_CORE_POLYMORPHIC_ARCHIVE_HPP
#define SF_CORE_POLYMORPHIC_ARCHIVE_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/ArchiveTraits.hpp>

#include <SF/Core/Serialization.hpp>

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
    using ArchiveType  = ioarchive_t;
    using key_type = ioarchive_t::key_type;

    static constexpr key_type max_key = archive_traits::max_key;

public:
    template <class T> static void save(core::ioarchive_t& archive, T& data)
    {
        call<oarchive_traits>(archive, data);
    }

    template <class T> static void load(core::ioarchive_t& archive, T& data)
    {
        call<iarchive_traits>(archive, data);
    }

private:
    template <class ArchiveType> struct is_valid_archive
        : std::integral_constant<bool, archive_traits_key_t<ArchiveType>::key != archive_traits::base_key> {};

private:
    template <template <key_type> class archive_traits,
              key_type Key, class T, SF_REQUIRE(Key == max_key)>
    static void call(core::ioarchive_t& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <key_type> class archive_traits,
              key_type Key = 0, class T, SF_REQUIRE(Key < max_key)>
    static void call(core::ioarchive_t& archive, T& data)
    {
        using DerivedArchive = typename archive_traits<Key>::type;

        if (archive_traits_key_t<DerivedArchive>::key == archive.trait)
            return try_call<DerivedArchive>(archive, data);

        call<archive_traits, Key + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(not is_valid_archive<DerivedArchive>::value)>
    static void try_call(core::ioarchive_t& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(is_valid_archive<DerivedArchive>::value)>
    static void try_call(core::ioarchive_t& archive, T& data)
    {
    #ifdef SF_DEBUG
        auto derived_archive = dynamic_cast<DerivedArchive*>(&archive);

        if (derived_archive == nullptr)
            throw "The read/write archive was registered incorrect.";
    #else
        auto derived_archive = static_cast<DerivedArchive*>(&archive);
    #endif // SF_DEBUG
        try_call_impl<DerivedArchive>(*derived_archive, data);
    }

    template <class DerivedArchive, class T>
    static void try_call_impl(iarchive_common_t& archive, T& data)
    {
        ::xxsf_load<T>(static_cast<DerivedArchive&>(archive), data);
    }
    template <class DerivedArchive, class T>
    static void try_call_impl(oarchive_common_t& archive, T& data)
    {
        ::xxsf_save<T>(static_cast<DerivedArchive&>(archive), data);
    }
};

template <typename T,
          SF_REQUIRE(meta::is_unsupported<T>::value)>
ioarchive_t& operator& (ioarchive_t& archive, T const& unsupported)
{
    static_assert(meta::to_false<T>(),
        "The 'T' is an unsupported type for the 'sf::ioarchive_t'.");

    return archive;
}

template <typename T,
          SF_REQUIRE(not meta::is_unsupported<T>::value)>
ioarchive_t& operator<< (ioarchive_t& archive, T const& data)
{
    polymorphic_archive_t::save(archive, const_cast<T&>(data));
    return archive;
}

template <typename T,
          SF_REQUIRE(not meta::is_unsupported<T>::value)>
ioarchive_t& operator>> (ioarchive_t& archive, T const& data)
{
    polymorphic_archive_t::load(archive, const_cast<T&>(data));
    return archive;
}

template <typename T,
          SF_REQUIRE(not meta::is_unsupported<T>::value)>
ioarchive_t& operator& (ioarchive_t& archive, T const& data)
{
    return archive.readonly ? archive >> data : archive << data;
}

} // namespace core

} // namespace sf

#endif // SF_CORE_POLYMORPHIC_ARCHIVE_HPP
