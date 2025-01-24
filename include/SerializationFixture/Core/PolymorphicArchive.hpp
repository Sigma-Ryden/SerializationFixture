#ifndef SF_CORE_POLYMORPHIC_ARCHIVE_HPP
#define SF_CORE_POLYMORPHIC_ARCHIVE_HPP

#ifdef SF_DEBUG
#include <typeinfo> // typeid
#endif // SD_DEBUG

#include <SerializationFixture/Core/ArchiveBase.hpp>
#include <SerializationFixture/Core/ArchiveTraits.hpp>

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Detail/Meta.hpp>
#include <SerializationFixture/Detail/MetaMacro.hpp>

namespace sf
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
              ::xxsf_archive_traits_key_type ArchiveKeyValue = 0,
              typename SerializableType>
    static void call(ioarchive_t& archive, SerializableType& data)
    {
        if constexpr (ArchiveKeyValue == ::xxsf_archive_traits_base_key)
        {
            throw "The read/write archive has invalid type key.";
        }
        else
        {
            using DerivedArchiveType = typename ArchiveRegistryTemplate<ArchiveKeyValue>::type;

            if (::xxsf_archive_traits<DerivedArchiveType>::key == archive.trait)
                return try_call<DerivedArchiveType>(archive, data);

            call<ArchiveRegistryTemplate, ArchiveKeyValue + 1>(archive, data);
        }
    }

    template <class DerivedArchiveType, typename SerializableType>
    static void try_call(ioarchive_t& archive, SerializableType& data)
    {
        if constexpr (::xxsf_archive_traits<DerivedArchiveType>::key == ::xxsf_archive_traits_base_key)
        {
            throw "The read/write archive was not registered.";
        }
        else
        {
        #ifdef SF_DEBUG
            if (typeid(archive) != typeid(DerivedArchiveType))
                throw "The read/write archive was registered incorrect.";
        #endif // SF_DEBUG
            try_call_impl<DerivedArchiveType>(static_cast<DerivedArchiveType&>(archive), data);
        }
    }

    template <class DerivedArchiveType, typename SerializableType,
              SF_REQUIRES(meta::is_oarchive<DerivedArchiveType>::value)>
    static void try_call_impl(ioarchive_t& archive, SerializableType& data)
    {
        ::xxsf<SerializableType>::save(static_cast<DerivedArchiveType&>(archive), data);
    }

    template <class DerivedArchiveType, typename SerializableType,
              SF_REQUIRES(meta::is_iarchive<DerivedArchiveType>::value)>
    static void try_call_impl(ioarchive_t& archive, SerializableType& data)
    {
        ::xxsf<SerializableType>::load(static_cast<DerivedArchiveType&>(archive), data);
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

} // namespace sf

#endif // SF_CORE_POLYMORPHIC_ARCHIVE_HPP
