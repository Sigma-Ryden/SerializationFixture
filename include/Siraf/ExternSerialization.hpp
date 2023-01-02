#ifndef SIRAF_EXTERN_SERIALIZATION_HPP
#define SIRAF_EXTERN_SERIALIZATION_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define EXTERN_CONDITIONAL_SERIALIZATION(mode, parameter, ...)                                          \
    template <class Archive, typename T,                                                                \
              SIREQUIRE(::siraf::meta::is_##mode<Archive>() and                                         \
                        ::siraf::meta::is_registered<T>() and                                           \
                        (bool)(__VA_ARGS__))>                                                           \
    Archive& operator& (Archive& archive, T& parameter)

#define EXTERN_SERIALIZATION(mode, parameter, ...)                                                      \
    template <class Archive,                                                                            \
              SIREQUIRE(::siraf::meta::is_##mode<Archive>() and                                         \
                        ::siraf::meta::is_registered<__VA_ARGS__>())>                                   \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

#endif // SIRAF_EXTERN_SERIALIZATION_HPP
