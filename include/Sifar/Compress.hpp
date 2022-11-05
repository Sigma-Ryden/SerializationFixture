#ifndef SIFAR_COMPRESS_HPP
#define SIFAR_COMPRESS_HPP

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define _SIFAR_HAS_FUNCTION_HELPER(name)                                                                \
    template <typename T, typename enable = void>                                                       \
    struct has_##name##_function : std::false_type {};                                                  \
    template <typename T>                                                                               \
    struct has_##name##_function<T, to_void<decltype(std::declval<T>().name())>>                        \
        : std::true_type {};

namespace sifar
{

namespace meta
{

_SIFAR_HAS_FUNCTION_HELPER(data)
_SIFAR_HAS_FUNCTION_HELPER(size)

template <typename T> constexpr bool is_container_class() noexcept
{
    return has_data_function<T>::value and has_size_function<T>::value;
}

template <typename T> constexpr bool is_container() noexcept
{
    return is_container_class<T>() or is_array<T>();
}

} // namespace meta

namespace compress
{

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and meta::is_class<T>() and
                    meta::is_compressible<T>())>
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
          SIREQUIRE(meta::is_archive<Archive>() and meta::is_array<T>() and
                    meta::is_compressible<T>())>
void fast(Archive& archive, T& array)
{
    archive.stream().call
    (
        array,
        utility::size(array) * sizeof(meta::value_type<T>)
    );
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and meta::is_container<T>())>
void slow(Archive& archive, T& data)
{
    for (auto& item : data)
        archive & item;
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and meta::is_container<T>() and
                    meta::is_compressible<T>())>
void zip(Archive& archive, T& data)
{
    fast(archive, data);
}

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and meta::is_container<T>() and
                    not meta::is_compressible<T>())>
void zip(Archive& archive, T& data)
{
    slow(archive, data);
}

} // namespace compress

} // namespace sifar

// clean up
#undef _SIFAR_HAS_FUNCTION_HELPER

#endif // SIFAR_COMPRESS_HPP
