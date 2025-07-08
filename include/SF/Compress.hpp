#ifndef SF_COMPRESS_HPP
#define SF_COMPRESS_HPP

#include <SF/Core/ArchiveBase.hpp>
#include <SF/Utility/Functional.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace compress
{

// always require compressible type for fast compression
template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
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
          SF_REQUIRES(meta::is_ioarchive<ArchiveType>::value)>
void slow(ArchiveType& archive, SerializableType& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       meta::is_compressible<SerializableType>>::value)>
void zip(ArchiveType& archive, SerializableType& object)
{
    fast(archive, object);
}

template <class ArchiveType, typename SerializableType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       std::negation<meta::is_compressible<SerializableType>>>::value)>
void zip(ArchiveType& archive, SerializableType& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace sf

#endif // SF_COMPRESS_HPP
