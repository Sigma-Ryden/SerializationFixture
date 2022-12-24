#ifndef SIRAF_SERIALIZABLE_HPP
#define SIRAF_SERIALIZABLE_HPP

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define _SERIALIZATION_CALL_DECLARATION(...)                                                            \
    template <class Archive> static void call(Archive& archive, __VA_ARGS__& self);

#define _SERIALIZATION_CALL_IMPLEMENTATION(mode, ...)                                                   \
    { __Serializable::mode<__VA_ARGS__>::call(archive, self); }

#define _SERIALIZATION_DECLARATION_IF(mode, ...)                                                        \
    template <typename T>                                                                               \
    struct __Serializable::mode<T, SIWHEN(__VA_ARGS__)> { _SERIALIZATION_CALL_DECLARATION(T) };

#define _SERIALIZATION_DECLARATION(mode, ...)                                                           \
    template <>                                                                                         \
    struct __Serializable::mode<__VA_ARGS__> { _SERIALIZATION_CALL_DECLARATION(__VA_ARGS__) };

#define _SERIALIZATION_IMPLEMENTATION_IF(mode, ...)                                                     \
    template <typename T>                                                                               \
    template <class Archive>                                                                            \
    void __Serializable::mode<T, SIWHEN(__VA_ARGS__)>::call(Archive& archive, T& self)

#define _SERIALIZATION_IMPLEMENTATION(mode, ...)                                                        \
    template <class Archive>                                                                            \
    void __Serializable::mode<__VA_ARGS__>::call(Archive& archive, __VA_ARGS__& self)

#define _CONDITIONAL_SERIALIZABLE_IMPLEMENTATION(mode, ...)                                             \
    _SERIALIZATION_DECLARATION_IF(mode, __VA_ARGS__)                                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(mode, __VA_ARGS__)

#define CONDITIONAL_SAVE_SERIALIZABLE(...)                                                              \
    _CONDITIONAL_SERIALIZABLE_IMPLEMENTATION(Save, __VA_ARGS__)

#define CONDITIONAL_LOAD_SERIALIZABLE(...)                                                              \
    _CONDITIONAL_SERIALIZABLE_IMPLEMENTATION(Load, __VA_ARGS__)

#define _SERIALIZABLE_IMPLEMENTATION(mode, ...)                                                         \
    _SERIALIZATION_DECLARATION(mode, __VA_ARGS__)                                                       \
    _SERIALIZATION_IMPLEMENTATION(mode, __VA_ARGS__)

#define SAVE_SERIALIZABLE(...)                                                                          \
    _SERIALIZABLE_IMPLEMENTATION(Save, __VA_ARGS__)

#define LOAD_SERIALIZABLE(...)                                                                          \
    _SERIALIZABLE_IMPLEMENTATION(Load, __VA_ARGS__)

#define CONDITIONAL_SAVE_LOAD_SERIALIZABLE(...)                                                         \
    _SERIALIZATION_DECLARATION_IF(Save, __VA_ARGS__)                                                    \
    CONDITIONAL_LOAD_SERIALIZABLE(__VA_ARGS__)                                                          \
    _SERIALIZATION_CALL_IMPLEMENTATION(Save, T, SIWHEN(__VA_ARGS__))                                    \
    _SERIALIZATION_IMPLEMENTATION_IF(Save, __VA_ARGS__)

#define SAVE_LOAD_SERIALIZABLE(...)                                                                     \
    _SERIALIZATION_DECLARATION(Save, __VA_ARGS__)                                                       \
    LOAD_SERIALIZABLE(__VA_ARGS__)                                                                      \
    _SERIALIZATION_CALL_IMPLEMENTATION(Save, __VA_ARGS__)                                               \
    _SERIALIZATION_IMPLEMENTATION(Save, __VA_ARGS__)

#ifdef SIRAF_SMART
    // Additional macro defs
    #define CONDITIONAL_SERIALIZATION(...) CONDITIONAL_SAVE_LOAD_SERIALIZABLE(__VA_ARGS__)
    #define SERIALIZATION(...) SAVE_LOAD_SERIALIZABLE(__VA_ARGS__)
#endif // SIRAF_SMART

class __Serializable
{
public:
    template <typename T, typename enable = void>
    struct Save { static constexpr bool pure = true; };

    template <typename T, typename enable = void>
    struct Load { static constexpr bool pure = true; };
};

namespace siraf
{

using Serializable = __Serializable;

} // namespace siraf

#endif // SIRAF_SERIALIZABLE_HPP
