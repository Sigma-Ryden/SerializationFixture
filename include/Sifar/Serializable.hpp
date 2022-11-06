#ifndef SIFAR_SERIALIZABLE_HPP
#define SIFAR_SERIALIZABLE_HPP

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define _SERIALIZATION_DECLARATION_IF(serialization_type, ...)                                          \
    template <typename T>                                                                               \
    struct SifarSerializable::serialization_type<T, SIWHEN(__VA_ARGS__)> {                              \
        template <class Archive>                                                                        \
        static void call(Archive& archive, T& self);                                                    \
    };

#define _SERIALIZATION_DECLARATION(serialization_type, ...)                                             \
    template <>                                                                                         \
    struct SifarSerializable::serialization_type<__VA_ARGS__, void> {                                   \
        template <class Archive>                                                                        \
        static void call(Archive& archive, __VA_ARGS__& self);                                          \
    };

#define _SERIALIZATION_IMPLEMENTATION_IF(serialization_type, ...)                                       \
    template <typename T>                                                                               \
    template <class Archive>                                                                            \
    void SifarSerializable::serialization_type<T, SIWHEN(__VA_ARGS__)>::call(                           \
        Archive& archive, T& self)

#define _SERIALIZATION_IMPLEMENTATION(serialization_type, ...)                                          \
    template <class Archive>                                                                            \
    void SifarSerializable::serialization_type<__VA_ARGS__, void>::call(                                \
        Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SAVE_SERIALIZABLE(...)                                                              \
    _SERIALIZATION_DECLARATION_IF(Save, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Save, __VA_ARGS__)

#define CONDITIONAL_LOAD_SERIALIZABLE(...)                                                              \
    _SERIALIZATION_DECLARATION_IF(Load, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Load, __VA_ARGS__)

#define _SERIALIZATION_CALL(serialization_type, ...)                                                    \
    { ::SifarSerializable::serialization_type<__VA_ARGS__>::call(archive, self); }

#define CONDITIONAL_SAVE_LOAD_SERIALIZABLE(...)                                                         \
    _SERIALIZATION_DECLARATION_IF(Save, __VA_ARGS__)                                                    \
    _SERIALIZATION_DECLARATION_IF(Load, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Load, __VA_ARGS__)                                                 \
    _SERIALIZATION_CALL(Save, T, SIWHEN(__VA_ARGS__))                                                   \
    _SERIALIZATION_IMPLEMENTATION_IF(Save, __VA_ARGS__)

#define SAVE_SERIALIZABLE(...)                                                                          \
    _SERIALIZATION_DECLARATION(Save, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(Save, __VA_ARGS__)

#define LOAD_SERIALIZABLE(...)                                                                          \
    _SERIALIZATION_DECLARATION(Load, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(Load, __VA_ARGS__)

#define SAVE_LOAD_SERIALIZABLE(...)                                                                     \
    _SERIALIZATION_DECLARATION(Save, __VA_ARGS__)                                                       \
    _SERIALIZATION_DECLARATION(Load, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(Load, __VA_ARGS__)                                                    \
    _SERIALIZATION_CALL(Save, __VA_ARGS__, void)                                                        \
    _SERIALIZATION_IMPLEMENTATION(Save, __VA_ARGS__)

#ifdef SIFAR_SMART
    // Additional macro defs
    #define CONDITIONAL_SERIALIZATION(...) CONDITIONAL_SAVE_LOAD_SERIALIZABLE(__VA_ARGS__)
    #define SERIALIZATION(...) SAVE_LOAD_SERIALIZABLE(__VA_ARGS__)
#endif // SIFAR_SMART

class SifarSerializable
{
public:
    template <typename T, typename enable = void>
    struct Save { static constexpr bool pure = true; };

    template <typename T, typename enable = void>
    struct Load { static constexpr bool pure = true; };
};

#endif // SIFAR_SERIALIZABLE_HPP
