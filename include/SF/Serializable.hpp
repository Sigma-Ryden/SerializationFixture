#ifndef SF_SERIALIZABLE_HPP
#define SF_SERIALIZABLE_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/AnyRegistry.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;                                                                       \
    friend class ::sf::Memory;

// Alternative instantiable registration with library trait no-rtti
#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        INSTANTIABLE_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename T> void serializable()
{
    static_assert(not meta::is_unsupported<T>::value, "The 'T' is an unsupported type for serialization.");
    static_assert(meta::is_registered<T>::value, "The 'T' is an unregistered type for serialization.");

    dynamic::InstantiableFixture<T>::call();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::AnyFixture<T>::call();
#endif // if
}

template <typename T> T&& serializable(T&& object)
{
    serializable<meta::decay<T>>();
    return std::forward<T>(object);
}

} // namepsace sf

#endif // SF_SERIALIZABLE_HPP
