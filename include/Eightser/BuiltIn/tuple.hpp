#ifndef EIGHTSER_BUILT_IN_TUPLE_HPP
#define EIGHTSER_BUILT_IN_TUPLE_HPP

#include <tuple> // tuple

#include <utility> // index_sequence, make_index_sequence

#include <Eightser/Core/Serialization.hpp>

namespace eightser
{

namespace detail
{

template <class ArchiveType, typename... ArgumentTypes, std::size_t... TupleElementIndexes>
void expand(ArchiveType& archive, std::tuple<ArgumentTypes...>& tuple, std::index_sequence<TupleElementIndexes...>)
{
    archive(std::get<TupleElementIndexes>(tuple)...);
}

} // namespace detail

} // namespace eightser

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename... ArgumentTypes>, std::tuple<ArgumentTypes...>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, tuple, template <typename... ArgumentTypes>, std::tuple<ArgumentTypes...>)
    SERIALIZATION
    (
        ::eightser::detail::expand(archive, tuple, std::make_index_sequence<sizeof...(ArgumentTypes)>{});
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_TUPLE_HPP
