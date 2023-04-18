#ifndef SF_CORE_SERIALIZATION_HPP
#define SF_CORE_SERIALIZATION_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/InstantiableTrait.hpp>

#include <SF/Core/Hash.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp> // SFWHEN, SFVOID

#define SERIALIZATION(mode, ...)                                                                        \
    template <>                                                                                         \
    struct Serialization::mode<__VA_ARGS__> {                                                           \
        template <class Archive>                                                                        \
        static void call(Archive& archive, __VA_ARGS__& self);                                          \
    };                                                                                                  \
    template <class Archive>                                                                            \
    void Serialization::mode<__VA_ARGS__>::call(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <typename T>                                                                               \
    struct Serialization::mode<T, SFWHEN(__VA_ARGS__)> {                                                \
        template <class Archive>                                                                        \
        static void call(Archive& archive, T& self);                                                    \
    };                                                                                                  \
    template <typename T> template <class Archive>                                                      \
    void Serialization::mode<T, SFWHEN(__VA_ARGS__)>::call(Archive& archive, T& self)

// Allow to hide implementationementation to translation unit, and declare interface in header
#define SERIALIZATION_INTERFACE(mode, ...)                                                              \
    template <>                                                                                         \
    struct Serialization::mode<__VA_ARGS__> {                                                           \
        static void call(sf::core::ArchiveBase& archive, __VA_ARGS__& self);                            \
    };

#define SERIALIZATION_IMPLEMENTATION(mode, ...)                                                         \
    void Serialization::mode<__VA_ARGS__>::call(sf::core::ArchiveBase& archive, __VA_ARGS__& self)

// should be in global namespace
class Serialization
{
private:
    // specificaly check for construction without arguments
    template <typename T, typename = void> struct has_implementation : std::false_type {};
    template <typename T> struct has_implementation<T, SFVOID(T{})> : std::true_type {};

public:
    template <class T> static constexpr bool is_save_class() noexcept
    {
        return has_implementation<Save<T>>::value;
    }

    template <class T> static constexpr bool is_load_class() noexcept
    {
        return has_implementation<Load<T>>::value;
    }

    template <class T>
    static constexpr bool is_saveload_class() noexcept
    {
        return has_implementation<SaveLoad<T>>::value;
    }

public:
    template <class T> static constexpr bool has_save_mode() noexcept
    {
        return is_save_class<T>() or is_saveload_class<T>();
    }

    template <class T> static constexpr bool has_load_mode() noexcept
    {
        return is_load_class<T>() or is_saveload_class<T>();
    }

public:
    template <typename T, typename enable = void> struct SaveLoad;
    template <typename T, typename enable = void> struct Save;
    template <typename T, typename enable = void> struct Load;

public:
    template <class Archive, typename T> static void call(Archive& archive, T& self)
    {
        // default implementationementation
        throw "The 'T' type cannot be saved/loaded.";
    }

private:
    template <typename T>
    struct SaveModeMeta
    {
        using array = sf::meta::type_array<Save<T>, SaveLoad<T>, Serialization>;
        static constexpr auto index = sf::meta::with(is_save_class<T>(), is_saveload_class<T>());

        using mode = typename array::template type<index>;
    };

    template <typename T>
    struct LoadModeMeta
    {
        using array = sf::meta::type_array<Load<T>, SaveLoad<T>, Serialization>;
        static constexpr auto index = sf::meta::with(is_load_class<T>(), is_saveload_class<T>());

        using mode = typename array::template type<index>;
    };

public:
    template <typename T> using SaveMode = typename SaveModeMeta<T>::mode;
    template <typename T> using LoadMode = typename LoadModeMeta<T>::mode;

private:
    template <typename C, typename = void> struct has_static_trait : std::false_type {};
    template <typename C> struct has_static_trait<C, SFVOID(&C::__static_trait)> : std::true_type {};

    template <typename C, typename = void> struct has_trait : std::false_type {};
    template <typename C> struct has_trait<C, SFVOID(&C::__trait)> : std::true_type {};

public:
    template <class T> static constexpr bool has_inner_trait() noexcept
    {
        return has_static_trait<T>::value and has_trait<T>::value;
    }

private:
    template <typename From, typename To, typename = void>
    struct can_static_cast : std::false_type {};

    template <typename From, typename To>
    struct can_static_cast<From, To, SFVOID(static_cast<To>(std::declval<From>()))>
    : std::true_type {};

    template <typename> static std::false_type is_returnable(...);
    template <typename T> static auto is_returnable(int) noexcept -> decltype
    (
        (void) static_cast<T(*)()>(nullptr),
        std::true_type{}
    );

    template <class, class> static std::false_type try_cast(...);
    template <class From, class To> static auto try_cast(int) -> decltype
    (
        (void) std::declval<void(&)(To)>()(std::declval<From>()),
        std::true_type{}
    );

public:
    template <class Base, class Derived> static constexpr bool is_virtual_base_of() noexcept
    {
        return sf::meta::is_base_of<Base, Derived>() and
               can_static_cast<Base*, Derived*>::value;
    }

    template <class From, class To> static constexpr bool is_cast_allowed() noexcept
    {
        return (decltype(is_returnable<To>(0))::value and decltype(try_cast<From, To>(0))::value)
            or (sf::meta::is_same_all<void, From, To>());
    }

    template <class From, class To> static constexpr bool is_pointer_cast_allowed() noexcept
    {
        return is_cast_allowed<From*, To*>()
            or sf::meta::is_void<From>();
    }

public:
    using ArchiveBase = sf::core::ArchiveBase;
    using InstantiableTraitBase = sf::core::InstantiableTraitBase;

public:
    template <typename Archive, typename T>
    static void save(Archive& archive, T& data)
    {
        SaveMode<T>::call(archive, data);
    }

    template <typename Archive, typename T>
    static void load(Archive& archive, T& data)
    {
        LoadMode<T>::call(archive, data);
    }

    template <typename Base, class Archive, typename Derived,
              SFREQUIRE(sf::meta::is_archive<Archive>() and
                        sf::meta::is_base_of<Base, Derived>())>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & cast<Base&>(object);
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>())>
    static InstantiableTraitBase::key_type trait(T& object) noexcept
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SFREQUIRE(has_inner_trait<T>())>
    static InstantiableTraitBase::key_type trait(T& object) noexcept
    {
        return object.__trait();
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>())>
    static InstantiableTraitBase::key_type static_trait() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SFREQUIRE(has_inner_trait<T>())>
    static constexpr InstantiableTraitBase::key_type static_trait() noexcept
    {
        return T::__static_trait();
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>())>
    static InstantiableTraitBase::key_type trait() noexcept
    {
        constexpr auto trait_key = sf::core::InstantiableTraitKey<T>::key;

        static_assert(trait_key == InstantiableTraitBase::base_key,
            "Export instantiable trait is not allowed using typeid.");

        return static_trait<T>();
    }

    template <class T, SFREQUIRE(has_inner_trait<T>())>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr InstantiableTraitBase::key_type trait() noexcept
#else
    static InstantiableTraitBase::key_type trait() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto trait_key = sf::core::InstantiableTraitKey<T>::key;

        return trait_key == InstantiableTraitBase::base_key
             ? static_trait<T>()
             : trait_key;
    }

private:
    template <class To, class From> static To cast(From& from) noexcept
    {
        return static_cast<To>(from);
    }

    template <class To, class From> static To runtime_cast(From& from) // may throw exception
    {
        return dynamic_cast<To>(from);
    }
};

#endif // SF_CORE_SERIALIZATION_HPP
