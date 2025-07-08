#ifndef SF_BUILT_IN_PAIR_HPP
#define SF_BUILT_IN_PAIR_HPP

#include <utility> // pair

#include <SF/Core/Serialization.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename FirstType, typename SecondType>), std::pair<FirstType, SecondType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, pair, (template <typename FirstType, typename SecondType>), std::pair<FirstType, SecondType>)
    SERIALIZATION
    (
       archive & pair.first & pair.second;
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_PAIR_HPP
