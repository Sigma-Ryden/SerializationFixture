#ifndef SF_SERIALIZABLE_HPP
#define SF_SERIALIZABLE_HPP

#include <SF/Core/Serialization.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/AnyRegistry.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend struct ::xxsf;                                                                               \
    template <typename, typename> friend struct ::xxsf_save;                                            \
    template <typename, typename> friend struct ::xxsf_load;                                            \
    template <typename, typename> friend struct ::xxsf_saveload;

// Alternative instantiable registration with library traits no-rtti
#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        SERIALIZATION_FIXTURE(__VA_ARGS__)                                                              \
        SERIALIZATION_TRAITS(__VA_ARGS__)
#endif // SERIALIZABLE

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename T> void serializable()
{
    static_assert(not meta::is_unsupported<T>::value, "The 'T' is an unsupported type for serialization.");
    static_assert(meta::is_registered_extern<T>::value, "The 'T' is an unregistered type for serialization.");

    dynamic::instantiable_fixture_t<T>::call();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::any_fixture_t<T>::call();
#endif // if
}

template <typename T> T&& serializable(T&& object)
{
    serializable<typename std::decay<T>::type>();
    return std::forward<T>(object);
}

} // namepsace sf

#endif // SF_SERIALIZABLE_HPP
