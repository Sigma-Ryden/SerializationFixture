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
    template <typename SerializableType>
    static void save(ioarchive_t& archive, SerializableType& data)
    {
        call<::xxsf_oarchive_registry>(archive, data);
    }

    template <typename SerializableType>
    static void load(ioarchive_t& archive, SerializableType& data)
    {
        call<::xxsf_iarchive_registry>(archive, data);
    }

private:
    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKeyValue,
              typename SerializableType,
              SF_REQUIRES(ArchiveKeyValue == ::xxsf_archive_traits_base_key)>
    static void call(ioarchive_t&, SerializableType&)
    {
        throw "The read/write archive has invalid type key.";
    }

    template <template <::xxsf_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxsf_archive_traits_key_type ArchiveKeyValue = 0,
              typename SerializableType,
              SF_REQUIRES(ArchiveKeyValue < ::xxsf_archive_traits_base_key)>
    static void call(core::ioarchive_t& archive, SerializableType& data)
    {
        using DerivedArchiveType = typename ArchiveRegistryTemplate<ArchiveKeyValue>::type;

        if (::xxsf_archive_traits<DerivedArchiveType>::key == archive.trait)
            return try_call<DerivedArchiveType>(archive, data);

        call<ArchiveRegistryTemplate, ArchiveKeyValue + 1>(archive, data);
    }

    template <class DerivedArchiveType, typename SerializableType,
              SF_REQUIRES(::xxsf_archive_traits<DerivedArchiveType>::key == ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t&, SerializableType&)
    {
        throw "The read/write archive was not registered.";
    }

    template <class DerivedArchiveType, typename SerializableType,
              SF_REQUIRES(::xxsf_archive_traits<DerivedArchiveType>::key != ::xxsf_archive_traits_base_key)>
    static void try_call(core::ioarchive_t& archive, SerializableType& data)
    {
    #ifdef SF_DEBUG
        if (typeid(archive) != typeid(DerivedArchiveType))
            throw "The read/write archive was registered incorrect.";
    #endif // SF_DEBUG
        try_call_impl<DerivedArchiveType>(static_cast<DerivedArchiveType&>(archive), data);
    }

    template <class DerivedArchiveType, typename SerializableType>
    static void try_call_impl(iarchive_common_t& archive, SerializableType& data)
    {
        ::xxsf_load<SerializableType>(static_cast<DerivedArchiveType&>(archive), data);
    }

    template <class DerivedArchiveType, typename SerializableType>
    static void try_call_impl(oarchive_common_t& archive, SerializableType& data)
    {
        ::xxsf_save<SerializableType>(static_cast<DerivedArchiveType&>(archive), data);
    }
};

template <typename SerializableType,
          SF_REQUIRES(meta::is_unsupported<SerializableType>::value)>
ioarchive_t& operator& (ioarchive_t& archive, SerializableType const&)
{
    static_assert(meta::to_false<SerializableType>(),
        "The 'SerializableType' is an unsupported type for the 'sf::ioarchive_t'.");

    return archive;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator<< (ioarchive_t& archive, SerializableType const& data)
{
    polymorphic_archive_t::save(archive, const_cast<SerializableType&>(data));
    return archive;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator>> (ioarchive_t& archive, SerializableType const& data)
{
    polymorphic_archive_t::load(archive, const_cast<SerializableType&>(data));
    return archive;
}

template <typename SerializableType,
          SF_REQUIRES(meta::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator& (ioarchive_t& archive, SerializableType const& data)
{
    return archive.readonly ? archive >> data : archive << data;
}

} // namespace core

} // namespace sf

#endif // SF_CORE_POLYMORPHIC_ARCHIVE_HPP
