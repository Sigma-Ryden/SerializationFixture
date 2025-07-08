#ifndef SF_SERIALIZABLE_HPP
#define SF_SERIALIZABLE_HPP

#include <SF/Core/Serialization.hpp>

#include <SF/Dynamic/InstantiableRegistry.hpp>
#include <SF/Dynamic/AnyRegistry.hpp>

#define SERIALIZABLE_ACCESS(...) \
    friend struct ::xxsf_cast_to_non_public_base; \
    template <typename, typename> friend struct ::xxsf;

namespace sf
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename SerializableType> void serializable()
{
    static_assert(std::negation<meta::is_unsupported<SerializableType>>::value,
        "The 'SerializableType' is an unsupported type for serialization.");

    dynamic::instantiable_registry.add<SerializableType>();

#if !defined(SF_ANY_SUPPORT_DISABLE)
    dynamic::any_registry.add<SerializableType>();
#endif // if
}

template <typename SerializableType> SerializableType&& serializable(SerializableType&& object)
{
    serializable<typename std::decay<SerializableType>::type>();
    return std::forward<SerializableType>(object);
}

} // namepsace sf

#endif // SF_SERIALIZABLE_HPP
