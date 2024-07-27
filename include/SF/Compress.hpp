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
template <class Archive, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               meta::is_compressible<T>>::value)>
void fast(Archive& archive, T& object)
{
    using item_type = typename meta::value_type<T>::type;

    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class Archive, typename T,
          SF_REQUIRE(meta::is_ioarchive<Archive>::value)>
void slow(Archive& archive, T& object)
{
    for (auto&& item : object)
        archive & item;
}

template <class Archive, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               meta::is_compressible<T>>::value)>
void zip(Archive& archive, T& object)
{
    fast(archive, object);
}

template <class Archive, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               meta::negation<meta::is_compressible<T>>>::value)>
void zip(Archive& archive, T& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace sf

#endif // SF_COMPRESS_HPP
