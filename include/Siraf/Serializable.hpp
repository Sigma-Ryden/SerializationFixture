#ifndef SIRAF_SERIALIZABLE_HPP
#define SIRAF_SERIALIZABLE_HPP

#include <Siraf/Core/Access.hpp>
#include <Siraf/Dynamic/FactoryTable.hpp>
#include <Siraf/Dynamic/Polymorphic.hpp>

#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        CLONEABLE_BODY(__VA_ARGS__)                                                                     \
        POLYMORPHIC_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

#endif // SIRAF_SERIALIZABLE_HPP
