#ifndef SIRAF_UNIFIED_DATA_HPP
#define SIRAF_UNIFIED_DATA_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(parameter, ...)                                         \
    template <class Archive, typename T,                                                                \
              SIREQUIRE(::siraf::meta::is_archive<Archive>() and                                        \
                        ::siraf::meta::is_registered<T>() and                                           \
                        (bool)(__VA_ARGS__))>                                                           \
    Archive& operator& (Archive& archive, T& parameter)

#define SAVE_LOAD_SERIALIZABLE_TYPE(parameter, ...)                                                     \
    template <class Archive,                                                                            \
              SIREQUIRE(::siraf::meta::is_archive<Archive>() and                                        \
                        ::siraf::meta::is_registered<__VA_ARGS__>())>                                   \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

#endif // SIRAF_UNIFIED_DATA_HPP
