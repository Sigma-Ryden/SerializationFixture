#ifndef EIGHTSER_SERIALIZABLE_HPP
#define EIGHTSER_SERIALIZABLE_HPP

#include <Eightser/Core/Serialization.hpp>

#include <Eightser/Dynamic/InstantiableRegistry.hpp>
#include <Eightser/Dynamic/AnyRegistry.hpp>

#define SERIALIZABLE_ACCESS(...) \
    friend struct ::xxeightser_cast_to_non_public_base; \
    template <typename, typename> friend struct ::xxeightser;

namespace eightser
{

// Type registry for instantiable & any serialization, allowed registered and supported types only
template <typename SerializableType> void serializable()
{
    static_assert(std::negation<meta::is_unsupported<SerializableType>>::value,
        "The 'SerializableType' is an unsupported type for serialization.");

    dynamic::instantiable_registry()->add<SerializableType>();

#if !defined(EIGHTSER_ANY_SUPPORT_DISABLE)
    dynamic::any_registry()->add<SerializableType>();
#endif // if
}

template <typename SerializableType> SerializableType&& serializable(SerializableType&& object)
{
    serializable<typename std::decay<SerializableType>::type>();
    return std::forward<SerializableType>(object);
}

} // namepsace eightser

#endif // EIGHTSER_SERIALIZABLE_HPP
