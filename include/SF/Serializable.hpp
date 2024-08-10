#ifndef SF_SERIALIZABLE_HPP
#define SF_SERIALIZABLE_HPP

#include <SF/Core/Serialization.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/AnyRegistry.hpp>

// TODO: remove xxsf
#define SERIALIZATION_ACCESS(...)                                                                       \
    friend struct ::xxsf_cast_to_non_public_base;                                                       \
    template <typename, typename> friend struct ::xxsf_save;                                            \
    template <typename, typename> friend struct ::xxsf_load;                                            \
    template <typename, typename> friend struct ::xxsf_saveload;

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename T> void serializable()
{
    static_assert(not meta::is_unsupported<T>::value, "The 'T' is an unsupported type for serialization.");

    dynamic::instantiable_registry_t::instance().add<T>();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::any_registry_t::instance().add<T>();
#endif // if
}

template <typename T> T&& serializable(T&& object)
{
    serializable<typename std::decay<T>::type>();
    return std::forward<T>(object);
}

} // namepsace sf

#endif // SF_SERIALIZABLE_HPP
