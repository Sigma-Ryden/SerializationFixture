#ifndef SIRAF_COMPRESS_HPP
#define SIRAF_COMPRESS_HPP

#include <Siraf/Utility/Utility.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace compress
{

// always require compressible type for fast compression
template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_compressible<T>())>
void fast(Archive& archive, T& object)
{
    using item_type = meta::value_type<T>;
    archive.stream().call
    (
        const_cast<item_type*>(utility::data(object)),
        utility::size(object) * sizeof(item_type)
    );
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>())>
void slow(Archive& archive, T& object)
{
    for (auto& item : object)
        archive & item;
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_compressible<T>())>
void zip(Archive& archive, T& object)
{
    fast(archive, object);
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and
                    not meta::is_compressible<T>())>
void zip(Archive& archive, T& object)
{
    slow(archive, object);
}

} // namespace compress

} // namespace siraf

#endif // SIRAF_COMPRESS_HPP
