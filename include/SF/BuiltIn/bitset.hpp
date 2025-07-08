#ifndef SF_BUILT_IN_BITSET_HPP
#define SF_BUILT_IN_BITSET_HPP

#include <bitset> // bitset

#include <SF/Core/Serialization.hpp>

// default array for bitset convertion
#include <SF/BuiltIn/string.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION(template <std::size_t SizeValue>, std::bitset<SizeValue>)
SERIALIZABLE_DECLARATION_INIT()

// slow impl
TEMPLATE_SERIALIZABLE(save, bitset, template <std::size_t SizeValue>, std::bitset<SizeValue>)
    SERIALIZATION
    (
        auto data = bitset.to_string();
        archive & data;
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, bitset, template <std::size_t SizeValue>, std::bitset<SizeValue>)
    SERIALIZATION
    (
        std::string data;
        archive & data;

        bitset = std::bitset<SizeValue>(data);
    )
SERIALIZABLE_INIT()

#endif // SF_BUILT_IN_BITSET_HPP
