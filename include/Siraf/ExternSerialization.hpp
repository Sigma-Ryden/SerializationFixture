#ifndef SIRAF_EXTERN_SERIALIZATION_HPP
#define SIRAF_EXTERN_SERIALIZATION_HPP

#include <Siraf/Core/TypeRegistry.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define EXTERN_CONDITIONAL_SERIALIZATION(mode, parameter, ...)                                          \
    template <class Archive, typename T,                                                                \
              SIREQUIRE(::siraf::meta::is_##mode<Archive>() and                                         \
                        ::siraf::meta::is_registered_extern<T>() and                                    \
                        (bool)(__VA_ARGS__))>                                                           \
    Archive& operator& (Archive& archive, T& parameter)

// require TYPE_REGISTRY before use if not def SIRAF_TYPE_REGISTRY_DISABLE
#define EXTERN_SERIALIZATION(mode, parameter, ...)                                                      \
    template <class Archive,                                                                            \
              SIREQUIRE(::siraf::meta::is_##mode<Archive>() and                                         \
                        ::siraf::meta::is_registered_extern<__VA_ARGS__>())>                            \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

#endif // SIRAF_EXTERN_SERIALIZATION_HPP
