#ifndef EIGHTSER_CORE_POLYMORPHIC_ARCHIVE_HPP
#define EIGHTSER_CORE_POLYMORPHIC_ARCHIVE_HPP

#ifdef EIGHTSER_DEBUG
#include <typeinfo> // typeid
#endif // SD_DEBUG

#include <Eightser/Core/ArchiveBase.hpp>
#include <Eightser/Core/ArchiveTraits.hpp>

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Detail/Meta.hpp>
#include <Eightser/Detail/MetaMacro.hpp>

namespace eightser
{

class polymorphic_archive_t
{
public:
    template <typename SerializableType>
    static void save(ioarchive_t& archive, SerializableType& data)
    {
        call<::xxeightser_oarchive_registry>(archive, data);
    }

    template <typename SerializableType>
    static void load(ioarchive_t& archive, SerializableType& data)
    {
        call<::xxeightser_iarchive_registry>(archive, data);
    }

private:
    template <template <::xxeightser_archive_traits_key_type> class ArchiveRegistryTemplate,
              ::xxeightser_archive_traits_key_type ArchiveKeyValue = 0,
              typename SerializableType>
    static void call(ioarchive_t& archive, SerializableType& data)
    {
        if constexpr (ArchiveKeyValue == ::xxeightser_archive_traits_base_key)
        {
            throw "The read/write archive has invalid type key.";
        }
        else
        {
            using DerivedArchiveType = typename ArchiveRegistryTemplate<ArchiveKeyValue>::type;

            if (::xxeightser_archive_traits<DerivedArchiveType>::key == archive.trait)
                return try_call<DerivedArchiveType>(archive, data);

            call<ArchiveRegistryTemplate, ArchiveKeyValue + 1>(archive, data);
        }
    }

    template <class DerivedArchiveType, typename SerializableType>
    static void try_call(ioarchive_t& archive, SerializableType& data)
    {
        if constexpr (::xxeightser_archive_traits<DerivedArchiveType>::key == ::xxeightser_archive_traits_base_key)
        {
            throw "The read/write archive was not registered.";
        }
        else
        {
        #ifdef EIGHTSER_DEBUG
            if (typeid(archive) != typeid(DerivedArchiveType))
                throw "The read/write archive was registered incorrect.";
        #endif // EIGHTSER_DEBUG
            try_call_impl<DerivedArchiveType>(static_cast<DerivedArchiveType&>(archive), data);
        }
    }

    template <class DerivedArchiveType, typename SerializableType,
              EIGHTSER_REQUIRES(meta::is_oarchive<DerivedArchiveType>::value)>
    static void try_call_impl(ioarchive_t& archive, SerializableType& data)
    {
        ::xxeightser<SerializableType>::save(static_cast<DerivedArchiveType&>(archive), data);
    }

    template <class DerivedArchiveType, typename SerializableType,
              EIGHTSER_REQUIRES(meta::is_iarchive<DerivedArchiveType>::value)>
    static void try_call_impl(ioarchive_t& archive, SerializableType& data)
    {
        ::xxeightser<SerializableType>::load(static_cast<DerivedArchiveType&>(archive), data);
    }
};

template <typename SerializableType,
          EIGHTSER_REQUIRES(meta::is_unsupported<SerializableType>::value)>
ioarchive_t& operator& (ioarchive_t& archive, SerializableType const&)
{
    static_assert(meta::to_false<SerializableType>(),
        "The 'SerializableType' is an unsupported type for the 'eightser::ioarchive_t'.");

    return archive;
}

template <typename SerializableType,
          EIGHTSER_REQUIRES(std::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator<< (ioarchive_t& archive, SerializableType const& data)
{
    polymorphic_archive_t::save(archive, const_cast<SerializableType&>(data));
    return archive;
}

template <typename SerializableType,
          EIGHTSER_REQUIRES(std::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator>> (ioarchive_t& archive, SerializableType const& data)
{
    polymorphic_archive_t::load(archive, const_cast<SerializableType&>(data));
    return archive;
}

template <typename SerializableType,
          EIGHTSER_REQUIRES(std::negation<meta::is_unsupported<SerializableType>>::value)>
ioarchive_t& operator& (ioarchive_t& archive, SerializableType const& data)
{
    return archive.readonly ? archive >> data : archive << data;
}

} // namespace eightser

#endif // EIGHTSER_CORE_POLYMORPHIC_ARCHIVE_HPP
