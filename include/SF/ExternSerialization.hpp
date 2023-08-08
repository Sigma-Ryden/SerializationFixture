#ifndef SF_EXTERN_SERIALIZATION_HPP
#define SF_EXTERN_SERIALIZATION_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/TypeRegistry.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

#define EXTERN_CONDITIONAL_SERIALIZATION(mode, parameter, ...)                                          \
    template <class Archive, typename T,                                                                \
              SFREQUIRE(::sf::meta::all<::sf::meta::is_##mode<Archive>,                                 \
                                        ::sf::meta::is_registered_extern<T>,                            \
                                        ::sf::meta::boolean<bool(__VA_ARGS__)>>::value)>                \
    Archive& operator& (Archive& archive, T& parameter)

// require TYPE_REGISTRY before use if not def SF_TYPE_REGISTRY_DISABLE
#define EXTERN_SERIALIZATION(mode, parameter, ...)                                                      \
    template <class Archive,                                                                            \
              SFREQUIRE(::sf::meta::all<::sf::meta::is_##mode<Archive>,                                 \
                                        ::sf::meta::is_registered_extern<__VA_ARGS__>>::value)>         \
    Archive& operator& (Archive& archive, __VA_ARGS__& parameter)

namespace sf
{

namespace meta
{

template <class T> struct is_Save : is_write_archive<T> {};
template <class T> struct is_Load : is_read_archive<T> {};

template <class T> struct is_SaveLoad : is_archive<T> {};

} // namespace meta

} // namespace sf

#endif // SF_EXTERN_SERIALIZATION_HPP
