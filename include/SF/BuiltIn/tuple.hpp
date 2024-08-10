#ifndef SF_BUILT_IN_TUPLE_HPP
#define SF_BUILT_IN_TUPLE_HPP

#include <tuple> // tuple

#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace detail
{

template <class ArchiveType, typename... ArgumentTypes, std::size_t... TupleElementIndexes>
void expand(ArchiveType& archive, std::tuple<ArgumentTypes...>& tuple, meta::index_sequence<TupleElementIndexes...>)
{
    archive(std::get<TupleElementIndexes>(tuple)...);
}

} // namespace detail

} // namespace sf

TEMPLATE_SERIALIZATION(saveload, tuple, (template <typename... ArgumentTypes>), std::tuple<ArgumentTypes...>)
{
    ::sf::detail::expand(archive, tuple, ::sf::meta::make_index_sequence<sizeof...(ArgumentTypes)>{});
}

#endif // SF_BUILT_IN_TUPLE_HPP
