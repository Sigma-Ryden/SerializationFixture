#ifndef SIFAR_UNIFIED_DATA_HPP
#define SIFAR_UNIFIED_DATA_HPP

#include <Sifar/Detail/Meta.hpp>

#define SERIALIZATION_UNIFIED_DATA(parameter, ...)                                                      \
    template <class Archive, typename T,                                                                \
              ::sifar::meta::require<::sifar::meta::is_archive<Archive>() and                           \
                                     ::sifar::meta::is_registered<T>() and                              \
                                     (bool)(__VA_ARGS__)> = 0>                                          \
    Archive& operator& (Archive& archive, T& parameter)

#endif // SIFAR_UNIFIED_DATA_HPP
