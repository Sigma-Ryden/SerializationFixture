#ifndef SIRAF_SERIALIZABLE_HPP
#define SIRAF_SERIALIZABLE_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/Memory.hpp>

#include <Siraf/Dynamic/FactoryTable.hpp>
#include <Siraf/Dynamic/Polymorphic.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;                                                                       \
    friend class ::siraf::Memory;                                                                       \
    friend class ::siraf::dynamic::FactoryTable;

#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        CLONEABLE_BODY(__VA_ARGS__)                                                                     \
        POLYMORPHIC_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

#endif // SIRAF_SERIALIZABLE_HPP
