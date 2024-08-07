#ifndef SF_CORE_POLYMORPHIC_ARCHIVE_HPP
#define SF_CORE_POLYMORPHIC_ARCHIVE_HPP

#ifdef SF_DEBUG
#include <typeinfo> // typeid
#endif // SD_DEBUG

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
    template <class T> static void save(ioarchive_t& archive, T& data)
    {
        call<::xxsf_oarchive_registry>(archive, data);
    }

    template <class T> static void load(ioarchive_t& archive, T& data)
    {
        call<::xxsf_iarchive_registry>(archive, data);
    }

private:
    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKey,
              class T, SF_REQUIRE(ArchiveKey == ::xxsf_archive_traits_base_key)>
    static void call(ioarchive_t& archive, T& data)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKey = 0, class T,
              SF_REQUIRE(ArchiveKey < ::xxsf_archive_traits_base_key)>
    static void call(core::ioarchive_t& archive, T& data)
    {
        using DerivedArchive = typename ArchiveRegistryTemplate<ArchiveKey>::type;

        if (::xxsf_archive_traits<DerivedArchive>::key == archive.trait)
            return try_call<DerivedArchive>(archive, data);

        call<ArchiveRegistryTemplate, ArchiveKey + 1>(archive, data);
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(::xxsf_archive_traits<DerivedArchive>::key == ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t& archive, T& data)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchive, class T,
              SF_REQUIRE(::xxsf_archive_traits<DerivedArchive>::key != ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t& archive, T& data)
    {
    #ifdef SF_DEBUG
        if (typeid(archive) != typeid(DerivedArchive))
            throw "The read/write archive was registered incorrect.";
    #endif // SF_DEBUG
        try_call_impl<DerivedArchive>(static_cast<DerivedArchive&>(archive), data);
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
