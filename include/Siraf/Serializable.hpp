#ifndef SIRAF_SERIALIZABLE_HPP
#define SIRAF_SERIALIZABLE_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/Memory.hpp>

#include <Siraf/Dynamic/InstantiableRegistry.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;                                                                       \
    friend class ::siraf::Memory;

#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        INSTANTIABLE_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

#endif // SIRAF_SERIALIZABLE_HPP
