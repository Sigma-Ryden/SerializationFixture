#ifndef SIFAR_UNIFIED_DATA_HPP
#define SIFAR_UNIFIED_DATA_HPP

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define SERIALIZATION_SAVE_LOAD_DATA(parameter, ...)                                                    \
    template <class Archive, typename T,                                                                \
              SIFAR_REQUIRE(::sifar::meta::is_archive<Archive>() and                                    \
                            ::sifar::meta::is_registered<T>() and                                       \
                            (bool)(__VA_ARGS__))>                                                       \
    Archive& operator& (Archive& archive, T& parameter)

#endif // SIFAR_UNIFIED_DATA_HPP
