#ifndef SF_BUILT_IN_BITSET_HPP
#define SF_BUILT_IN_BITSET_HPP

#include <bitset> // bitset

#include <SF/Core/Serialization.hpp>

// default array for bitset convertion
#include <SF/BuiltIn/string.hpp>

// slow impl
TEMPLATE_SERIALIZATION(save, bitset, template <std::size_t SizeValue>, std::bitset<SizeValue>)
{
    auto data = bitset.to_string();
    archive & data;
}

TEMPLATE_SERIALIZATION(load, bitset, template <std::size_t SizeValue>, std::bitset<SizeValue>)
{
    std::string data;
    archive & data;

    bitset = std::bitset<SizeValue>(data);
}

#endif // SF_BUILT_IN_BITSET_HPP
