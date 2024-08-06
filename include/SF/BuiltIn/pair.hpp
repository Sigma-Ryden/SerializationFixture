#ifndef SF_BUILT_IN_PAIR_HPP
#define SF_BUILT_IN_PAIR_HPP

#include <utility> // pair

#include <SF/Core/Serialization.hpp>

TEMPLATE_SERIALIZATION(saveload, pair, (template <typename FirstType, typename SecondType>), std::pair<FirstType, SecondType>)
{
    archive & pair.first & pair.second;
}

#endif // SF_BUILT_IN_PAIR_HPP
