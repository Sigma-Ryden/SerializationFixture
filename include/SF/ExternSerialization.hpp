#ifndef SF_EXTERN_SERIALIZATION_HPP
#define SF_EXTERN_SERIALIZATION_HPP

#include <SF/Core/TypeRegistry.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

#define EXTERN_CONDITIONAL_SERIALIZATION(mode, parameter, ...)                                          \
    template <class Archive, typename T,                                                                \
              SIREQUIRE(::sf::meta::is_##mode<Archive>() and                                            \
                        ::sf::meta::is_registered_extern<T>() and                                       \
                        (bool)(__VA_ARGS__))>                                                           \
    Archive& operator& (Archive& archive, T& parameter)

// require TYPE_REGISTRY before use if not def SF_TYPE_REGISTRY_DISABLE
#define EXTERN_SERIALIZATION(mode, parameter, ...)                                                      \
    template <class Archive,                                                                            \
              SIREQUIRE(::sf::meta::is_##mode<Archive>() and                                            \
                        ::sf::meta::is_registered_extern<__VA_ARGS__>())>                               \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

#endif // SF_EXTERN_SERIALIZATION_HPP
