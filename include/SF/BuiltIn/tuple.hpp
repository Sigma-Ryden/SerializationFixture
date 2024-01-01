#ifndef SF_BUILT_IN_TUPLE_HPP
#define SF_BUILT_IN_TUPLE_HPP

#include <type_traits> // true_type, false_type

#include <tuple> // tuple

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_tuple : std::false_type {};
template <typename... Tn>
struct is_std_tuple<std::tuple<Tn...>> : std::true_type {};

} // namespace meta

namespace detail
{

template <class Archive, class T, std::size_t... I,
          SFREQUIRE(meta::is_std_tuple<T>::value)>
void expand(Archive& archive, T& tuple, meta::index_sequence<I...>)
{
    archive(std::get<I>(tuple)...);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, tuple, meta::is_std_tuple<T>::value)
{
    constexpr auto size = std::tuple_size<T>::value;
    detail::expand(archive, tuple, meta::make_index_sequence<size>{});

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_tuple<T>::value)

#endif // SF_BUILT_IN_TUPLE_HPP
