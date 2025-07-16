#ifndef EIGHTSER_COMPRESS_HPP
#define EIGHTSER_COMPRESS_HPP

#include <Eightser/Core/ArchiveBase.hpp>
#include <Eightser/Utility/Functional.hpp>

#include <Eightser/Detail/Meta.hpp>
#include <Eightser/Detail/MetaMacro.hpp>

namespace eightser
{

namespace compress
{

// always require compressible type for fast compression
template <class ArchiveType, typename SerializableType,
          EIGHTSER_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       meta::is_compressible<SerializableType>>::value)>
void fast(ArchiveType& archive, SerializableType& object)
{
    using item_type = typename meta::value<SerializableType>::type;

    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class ArchiveType, typename SerializableType,
          EIGHTSER_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
void slow(ArchiveType& archive, SerializableType& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class ArchiveType, typename SerializableType,
          EIGHTSER_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       meta::is_compressible<SerializableType>>::value)>
void zip(ArchiveType& archive, SerializableType& object)
{
    fast(archive, object);
}

template <class ArchiveType, typename SerializableType,
          EIGHTSER_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       std::negation<meta::is_compressible<SerializableType>>>::value)>
void zip(ArchiveType& archive, SerializableType& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace eightser

#endif // EIGHTSER_COMPRESS_HPP
