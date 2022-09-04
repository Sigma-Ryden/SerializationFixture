#ifndef SIFAR_SUPPORT_TUPLE_HPP
#define SIFAR_SUPPORT_TUPLE_HPP

#include <type_traits> // true_type, false_type

#include <tuple> // tuple

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/UnifiedData.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_tuple : std::false_type {};
template <typename... Tn>
struct is_std_tuple<std::tuple<Tn...>> : std::true_type {};

} // namespace meta

namespace library
{

namespace detail
{

template <class Archive, class Tuple, std::size_t... I>
void expand_impl(Archive& archive, Tuple& tuple, meta::index_sequence<I...>)
{
    archive(std::get<I>(tuple)...);
}

} // namespace detail

template <class Archive, class T,
          meta::require<meta::is_read_archive<Archive>() or
                        meta::is_write_archive<Archive>()> = 0,
          meta::require<meta::is_std_tuple<T>::value> = 0>
void expand(Archive& archive, T& tuple)
{
    constexpr auto N = std::tuple_size<T>::value;
    detail::expand_impl(archive, tuple, meta::make_index_sequence<N>{});
}

SERIALIZATION_UNIFIED_DATA(tuple, meta::is_std_tuple<T>::value)
{
    expand(archive, tuple);

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_tuple<T>::value)

#endif // SIFAR_SUPPORT_TUPLE_HPP
