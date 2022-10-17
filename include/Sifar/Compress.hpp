#ifndef SIFAR_COMPRESS_HPP
#define SIFAR_COMPRESS_HPP

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace compress
{

template <class Archive, typename T,
          SIFAR_REQUIRE(meta::is_archive<Archive>() and
                        meta::is_class<T>() and meta::is_compressible<T>())>
void fast(Archive& archive, T& object)
{
    using item_type = meta::value_type<T>;
    archive.stream().call
    (
        const_cast<item_type*>(object.data()),
        object.size() * sizeof(item_type)
    );
}

template <class Archive, typename T,
          SIFAR_REQUIRE(meta::is_archive<Archive>() and
                        meta::is_array<T>() and meta::is_compressible<T>())>
void fast(Archive& archive, T& array)
{
    archive.stream().call
    (
        array,
        utility::size(array) * sizeof(meta::value_type<T>)
    );
}

template <class Archive, typename T,
          SIFAR_REQUIRE(meta::is_archive<Archive>())>
void slow(Archive& archive, T& data)
{
    for (auto& item : data)
        archive & item;
}

template <class Archive, typename T,
          SIFAR_REQUIRE(meta::is_archive<Archive>() and
                       (meta::is_class<T>() or meta::is_array<T>()) and
                        meta::is_compressible<T>())>
void zip(Archive& archive, T& data)
{
    fast(archive, data);
}

template <class Archive, typename T,
          SIFAR_REQUIRE(meta::is_archive<Archive>() and
                       (meta::is_class<T>() or meta::is_array<T>()) and not
                        meta::is_compressible<T>())>
void zip(Archive& archive, T& data)
{
    slow(archive, data);
}

} // namespace compress

} // namespace sifar

#endif // SIFAR_COMPRESS_HPP
