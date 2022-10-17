#ifndef SIFAR_SERIALIZABLE_HPP
#define SIFAR_SERIALIZABLE_HPP

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define _SERIALIZATION_DECLARATION_IF(serialization_type, ...)                                          \
    template <typename T>                                                                               \
    struct SifarSerializable::serialization_type<T, SIFAR_WHEN(__VA_ARGS__)> {                          \
        using type = T;                                                                                 \
        template <class Archive> static void call(Archive& archive, type& self);                        \
    };

#define _SERIALIZATION_DECLARATION(serialization_type, ...)                                             \
    template <>                                                                                         \
    struct SifarSerializable::serialization_type<__VA_ARGS__, void> {                                   \
        using type = __VA_ARGS__;                                                                       \
        template <class Archive> static void call(Archive& archive, type& self);                        \
    };

#define _SERIALIZATION_IMPLEMENTATION_IF(serialization_type, ...)                                       \
    template <typename T>                                                                               \
    template <class Archive>                                                                            \
    void SifarSerializable::serialization_type<T, SIFAR_WHEN(__VA_ARGS__)>::call(                       \
        Archive& archive, type& self)

#define _SERIALIZATION_IMPLEMENTATION(serialization_type, ...)                                          \
    template <class Archive>                                                                            \
    void SifarSerializable::serialization_type<__VA_ARGS__, void>::call(                                \
        Archive& archive, type& self)

#define SERIALIZATION_CONDITIONAL_SAVE(...)                                                             \
    _SERIALIZATION_DECLARATION_IF(Save, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Save, __VA_ARGS__)

#define SERIALIZATION_CONDITIONAL_LOAD(...)                                                             \
    _SERIALIZATION_DECLARATION_IF(Load, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Load, __VA_ARGS__)

#define SERIALIZATION_CONDITIONAL_SAVE_LOAD(...)                                                        \
    _SERIALIZATION_DECLARATION_IF(Save, __VA_ARGS__)                                                    \
    _SERIALIZATION_DECLARATION_IF(Load, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Load, __VA_ARGS__)                                                 \
    { ::SifarSerializable::Save<T, SIFAR_WHEN(__VA_ARGS__)>::call(archive, self); }                     \
    _SERIALIZATION_IMPLEMENTATION_IF(Save, __VA_ARGS__)

#define SERIALIZATION_SAVE(...)                                                                         \
    _SERIALIZATION_DECLARATION(Save, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(Save, __VA_ARGS__)

#define SERIALIZATION_LOAD(...)                                                                         \
    _SERIALIZATION_DECLARATION(Load, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(Load, __VA_ARGS__)

#define SERIALIZATION_SAVE_LOAD(...)                                                                    \
    _SERIALIZATION_DECLARATION(Save, __VA_ARGS__)                                                       \
    _SERIALIZATION_DECLARATION(Load, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(Load, __VA_ARGS__)                                                    \
    { ::SifarSerializable::Save<__VA_ARGS__, void>::call(archive, self); }                              \
    _SERIALIZATION_IMPLEMENTATION(Save, __VA_ARGS__)

#ifdef SIFAR_SMART
    // Additional macro defs
    #define SERIALIZATION_CONDITIONAL(...) SERIALIZATION_CONDITIONAL_SAVE_LOAD(__VA_ARGS__)
    #define SERIALIZATION(...) SERIALIZATION_SAVE_LOAD(__VA_ARGS__)
#endif // SIFAR_SMART

class SifarSerializable
{
public:
    template <typename T, typename enable = void>
    struct Save { using type = void; };

    template <typename T, typename enable = void>
    struct Load { using type = void; };
};

#endif // SIFAR_SERIALIZABLE_HPP
