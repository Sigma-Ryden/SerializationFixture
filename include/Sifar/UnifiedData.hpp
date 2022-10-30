#ifndef SIFAR_UNIFIED_DATA_HPP
#define SIFAR_UNIFIED_DATA_HPP

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(parameter, ...)                                         \
    template <class Archive, typename T,                                                                \
              SIREQUIRE(::sifar::meta::is_archive<Archive>() and                                        \
                        ::sifar::meta::is_registered<T>() and                                           \
                        (bool)(__VA_ARGS__))>                                                           \
    Archive& operator& (Archive& archive, T& parameter)

#define SAVE_LOAD_SERIALIZABLE_TYPE(parameter, ...)                                                     \
    template <class Archive,                                                                            \
              SIREQUIRE(::sifar::meta::is_archive<Archive>() and                                        \
                        ::sifar::meta::is_registered<__VA_ARGS__>())>                                   \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

#endif // SIFAR_UNIFIED_DATA_HPP
