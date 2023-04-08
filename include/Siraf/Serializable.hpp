#ifndef SIRAF_SERIALIZABLE_HPP
#define SIRAF_SERIALIZABLE_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/Memory.hpp>

#include <Siraf/Dynamic/InstantiableRegistry.hpp>
#include <Siraf/Dynamic/AnyRegistry.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;                                                                       \
    friend class ::siraf::Memory;

// Alternative instantiable registration with library trait no-rtti
#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        INSTANTIABLE_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

namespace siraf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename T> void serializable()
{
    static_assert(not meta::is_unsupported<T>(), "The 'T' is an unsupported type for serialization.");
    static_assert(meta::is_registered<T>(), "The 'T' is an unregistered type for serialization.");

    dynamic::InstantiableFixture<T>::call();

#if __cplusplus >= 201703L && !defined(SIRAF_ANY_SUPPORT_DISABLE)
    dynamic::AnyFixture<T>::call();
#endif // if
}

template <typename T> T&& serializable(T&& object)
{
    serializable<meta::decay<T>>();
    return std::forward<T>(object);
}

} // namepsace siraf

#endif // SIRAF_SERIALIZABLE_HPP
