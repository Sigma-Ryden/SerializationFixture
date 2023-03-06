#ifndef SIRAF_CORE_SERIALIZATION_BRIDGE_HPP
#define SIRAF_CORE_SERIALIZATION_BRIDGE_HPP

#include <Siraf/Core/Access.hpp>
#include <Siraf/Core/ArchiveBase.hpp>

#include <Siraf/Detail/MetaMacro.hpp> // SIWHEN

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::SerializationBridge;                                                                 \
    friend class ::siraf::core::Access;

#define SERIALIZATION(mode, ...)                                                                        \
    template <> struct SerializationBridge::mode<__VA_ARGS__> {                                         \
        template <class Archive> static void call(Archive& archive, __VA_ARGS__& self);                 \
    };                                                                                                  \
    template <class Archive>                                                                            \
    void SerializationBridge::mode<__VA_ARGS__>::call(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <typename T> struct SerializationBridge::mode<T, SIWHEN(__VA_ARGS__)> {                    \
        template <class Archive> static void call(Archive& archive, T& self);                           \
    };                                                                                                  \
    template <typename T> template <class Archive>                                                      \
    void SerializationBridge::mode<T, SIWHEN(__VA_ARGS__)>::call(Archive& archive, T& self)

// should be in global namespace
class SerializationBridge
{
public:
    template <typename T, typename enable = void>
    struct SaveLoad { static constexpr bool pure = true; };

    template <typename T, typename enable = void>
    struct Save { static constexpr bool pure = true; };

    template <typename T, typename enable = void>
    struct Load { static constexpr bool pure = true; };

public:
    // default implementation
    template <class Archive, typename T>
    static void call(Archive& archive, T& self)
    {
        throw "The 'T' type cannot be saved/loaded.";
    }
};

#endif // SIRAF_CORE_SERIALIZATION_BRIDGE_HPP
