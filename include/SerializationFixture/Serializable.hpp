#ifndef SF_SERIALIZABLE_HPP
#define SF_SERIALIZABLE_HPP

#include <SerializationFixture/Core/Serialization.hpp>

#include <SerializationFixture/Dynamic/InstantiableRegistry.hpp>
#include <SerializationFixture/Dynamic/AnyRegistry.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend struct ::xxsf_cast_to_non_public_base;                                                       \
    template <typename, typename> friend struct ::xxsf_save;                                            \
    template <typename, typename> friend struct ::xxsf_load;                                            \
    template <typename, typename> friend struct ::xxsf_saveload;

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename SerializableType> void serializable()
{
    static_assert(meta::negation<meta::is_unsupported<SerializableType>>::value,
        "The 'SerializableType' is an unsupported type for serialization.");

    dynamic::instantiable_registry_t::instance().add<SerializableType>();

#if __cplusplus >= 201703L && !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::any_registry_t::instance().add<SerializableType>();
#endif // if
}

template <typename SerializableType> SerializableType&& serializable(SerializableType&& object)
{
    serializable<typename std::decay<SerializableType>::type>();
    return std::forward<SerializableType>(object);
}

} // namepsace sf

#endif // SF_SERIALIZABLE_HPP
