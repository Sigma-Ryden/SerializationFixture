#ifndef SF_BUILT_IN_TUPLE_HPP
#define SF_BUILT_IN_TUPLE_HPP

#include <tuple> // tuple

#include <utility> // index_sequence, make_index_sequence

#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace detail
{

template <class ArchiveType, typename... ArgumentTypes, std::size_t... TupleElementIndexes>
void expand(ArchiveType& archive, std::tuple<ArgumentTypes...>& tuple, std::index_sequence<TupleElementIndexes...>)
{
    archive(std::get<TupleElementIndexes>(tuple)...);
}

} // namespace detail

} // namespace sf

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename... ArgumentTypes>, std::tuple<ArgumentTypes...>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, tuple, template <typename... ArgumentTypes>, std::tuple<ArgumentTypes...>)
    SERIALIZATION
    (
        ::sf::detail::expand(archive, tuple, std::make_index_sequence<sizeof...(ArgumentTypes)>{});
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_TUPLE_HPP
