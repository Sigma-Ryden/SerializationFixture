#ifndef SIRAF_COMPRESS_HPP
#define SIRAF_COMPRESS_HPP

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>

#include <Siraf/Utility.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace compress
{

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
          SIREQUIRE(meta::is_archive<Archive>() and
                    not meta::is_compressible<T>())>
void slow(Archive& archive, T& data)
{
    for (auto& item : data)
        archive & item;
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_compressible<T>())>
void zip(Archive& archive, T& data)
{
    fast(archive, data);
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and
                    not meta::is_compressible<T>())>
void zip(Archive& archive, T& data)
{
    slow(archive, data);
}

} // namespace compress

} // namespace siraf

// clean up
#undef _SIRAF_HAS_FUNCTION_HELPER

#endif // SIRAF_COMPRESS_HPP
