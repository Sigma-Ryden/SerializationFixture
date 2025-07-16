#ifndef EIGHTSER_BUILT_IN_PAIR_HPP
#define EIGHTSER_BUILT_IN_PAIR_HPP

#include <utility> // pair

#include <Eightser/Core/Serialization.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION((template <typename FirstType, typename SecondType>), std::pair<FirstType, SecondType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, pair, (template <typename FirstType, typename SecondType>), std::pair<FirstType, SecondType>)
    SERIALIZATION
    (
       archive & pair.first & pair.second;
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_PAIR_HPP
