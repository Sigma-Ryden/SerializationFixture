#ifndef SF_COMPRESS_HPP
#define SF_COMPRESS_HPP

#include <SF/Core/SerializatonBase.hpp>

#include <SF/Utility/Functional.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace compress
{

// always require compressible type for fast compression
template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_compressible<T>>::value)>
void fast(ArchiveType& archive, T& object)
{
    using item_type = typename meta::value<T>::type;

    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::is_ioarchive<ArchiveType>::value)>
void slow(ArchiveType& archive, T& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_compressible<T>>::value)>
void zip(ArchiveType& archive, T& object)
{
    fast(archive, object);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::negation<meta::is_compressible<T>>>::value)>
void zip(ArchiveType& archive, T& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace sf

#endif // SF_COMPRESS_HPP
