#ifndef EIGHTSER_UNION_HPP
#define EIGHTSER_UNION_HPP

#include <Eightser/Core/Serialization.hpp>
#include <Eightser/Binary.hpp>

CONDITIONAL_SERIALIZABLE_DECLARATION(std::is_union<S>::value)
SERIALIZABLE_DECLARATION_INIT()

CONDITIONAL_SERIALIZABLE(saveload, data, std::is_union<S>::value)
    SERIALIZATION
    (
        ::eightser::binary(archive, data);
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_UNION_HPP
