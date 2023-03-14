#ifndef SIRAF_CORE_SERIALIZATION_HPP
#define SIRAF_CORE_SERIALIZATION_HPP

#include <Siraf/Core/ArchiveBase.hpp>
#include <Siraf/Core/TypeRegistry.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp> // SIWHEN

#define SERIALIZATION(mode, ...)                                                                        \
    template <> struct Serialization::mode<__VA_ARGS__> {                                               \
        template <class Archive> static void call(Archive& archive, __VA_ARGS__& self);                 \
    };                                                                                                  \
    template <class Archive>                                                                            \
    void Serialization::mode<__VA_ARGS__>::call(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <typename T> struct Serialization::mode<T, SIWHEN(__VA_ARGS__)> {                          \
        template <class Archive> static void call(Archive& archive, T& self);                           \
    };                                                                                                  \
    template <typename T> template <class Archive>                                                      \
    void Serialization::mode<T, SIWHEN(__VA_ARGS__)>::call(Archive& archive, T& self)

// should be in global namespace
class Serialization
{
private:
    // specificaly check for construction without arguments
    template <typename T, typename = void>
    struct has_implementation : std::false_type {};
    template <typename T>
    struct has_implementation<T, ::siraf::meta::to_void<decltype(T{})>> : std::true_type {};

public:
    template <class T>
    static constexpr bool is_save_class() noexcept
    {
        return has_implementation<::Serialization::Save<T>>::value;
    }

    template <class T>
    static constexpr bool is_load_class() noexcept
    {
        return has_implementation<::Serialization::Load<T>>::value;
    }

    template <class T>
    static constexpr bool is_saveload_class() noexcept
    {
        return has_implementation<::Serialization::SaveLoad<T>>::value;
    }

public:
    template <class T>
    static constexpr bool has_save_mode() noexcept
    {
        return is_save_class<T>() or is_saveload_class<T>();
    }

    template <class T>
    static constexpr bool has_load_mode() noexcept
    {
        return is_load_class<T>() or is_saveload_class<T>();
    }

public:
    template <typename T, typename enable = void> struct SaveLoad;
    template <typename T, typename enable = void> struct Save;
    template <typename T, typename enable = void> struct Load;

public:
    // default implementation
    template <class Archive, typename T>
    static void call(Archive& archive, T& self)
    {
        throw "The 'T' type cannot be saved/loaded.";
    }

private:
    template <typename T>
    struct SaveModeMeta
    {
        using scope = siraf::meta::scope<Save<T>, SaveLoad<T>, Serialization>;
        static constexpr auto index = siraf::meta::select(is_save_class<T>(), is_saveload_class<T>());

        using mode = typename scope::template type<index>;
    };

    template <typename T>
    struct LoadModeMeta
    {
        using scope = siraf::meta::scope<Load<T>, SaveLoad<T>, Serialization>;
        static constexpr auto index = siraf::meta::select(is_load_class<T>(), is_saveload_class<T>());

        using mode = typename scope::template type<index>;
    };

public:
    template <typename T> using SaveMode = typename SaveModeMeta<T>::mode;
    template <typename T> using LoadMode = typename LoadModeMeta<T>::mode;
};

CONDITIONAL_TYPE_REGISTRY(::Serialization::is_save_class<T>() or ::Serialization::is_load_class<T>())
CONDITIONAL_TYPE_REGISTRY(::Serialization::is_saveload_class<T>())

#endif // SIRAF_CORE_SERIALIZATION_HPP
