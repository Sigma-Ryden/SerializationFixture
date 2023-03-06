#ifndef SIRAF_SUPPORT_TUPLE_HPP
#define SIRAF_SUPPORT_TUPLE_HPP

#include <type_traits> // true_type, false_type

#include <tuple> // tuple

#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/ExternSerialization.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_tuple : std::false_type {};
template <typename... Tn>
struct is_std_tuple<std::tuple<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <class Archive, class Tuple, std::size_t... I>
void expand_impl(Archive& archive, Tuple& tuple, meta::index_sequence<I...>)
{
    archive(std::get<I>(tuple)...);
}

} // namespace detail

inline namespace library
{

template <class Archive, class T,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_std_tuple<T>::value)>
void expand(Archive& archive, T& tuple)
{
    constexpr auto N = std::tuple_size<T>::value;
    detail::expand_impl(archive, tuple, meta::make_index_sequence<N>{});
}

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, tuple, meta::is_std_tuple<T>::value)
{
    expand(archive, tuple);
    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_tuple<T>::value)

#endif // SIRAF_SUPPORT_TUPLE_HPP
