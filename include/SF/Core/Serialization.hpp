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
        template <class Archive> mode(Archive&, __VA_ARGS__&);                                          \
    };                                                                                                  \
    template <class Archive>                                                                            \
    Serialization::mode<__VA_ARGS__>::mode(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <class T>                                                                                  \
    struct Serialization::mode<T, SFWHEN(__VA_ARGS__)> {                                                \
        template <class Archive> mode(Archive&, T&);                                                    \
    };                                                                                                  \
    template <class T> template <class Archive>                                                         \
    Serialization::mode<T, SFWHEN(__VA_ARGS__)>::mode(Archive& archive, T& self)

// Allow to hide implementationementation to translation unit, and declare interface in header
#define SERIALIZATION_INTERFACE(mode, ...)                                                              \
    template <>                                                                                         \
    struct Serialization::mode<__VA_ARGS__> {                                                           \
        mode(sf::core::IOArchive&, __VA_ARGS__&);                                                       \
    };

#define SERIALIZATION_IMPLEMENTATION(mode, ...)                                                         \
    Serialization::mode<__VA_ARGS__>::mode(sf::core::IOArchive& archive, __VA_ARGS__& self)

// should be in global namespace
class Serialization
{
public:
    template <class T, typename enable = void> struct SaveLoad;
    template <class T, typename enable = void> struct Save;
    template <class T, typename enable = void> struct Load;

public:
    template <class T> struct is_save_class : sf::meta::is_complete<Save<T>> {};
    template <class T> struct is_load_class : sf::meta::is_complete<Load<T>> {};
    template <class T> struct is_saveload_class : sf::meta::is_complete<SaveLoad<T>> {};

public:
    template <class T> struct has_save_mode : sf::meta::one<is_save_class<T>, is_saveload_class<T>> {};
    template <class T> struct has_load_mode : sf::meta::one<is_load_class<T>, is_saveload_class<T>> {};

public:
    template <class Archive, class T> static void call(Archive& archive, T& self)
    {
        throw "The 'T' type cannot be saved/loaded."; // default implementation
    }

private:
    template <class T>
    struct SaveModeMeta
    {
        static constexpr auto index = sf::meta::with<is_save_class<T>, is_saveload_class<T>>::value;

        using mode_array = sf::meta::type_array<Save<T>, SaveLoad<T>, Serialization>;
        using mode = typename mode_array::template type<index>;
    };

    template <class T>
    struct LoadModeMeta
    {
        static constexpr auto index = sf::meta::with<is_load_class<T>, is_saveload_class<T>>::value;

        using mode_array = sf::meta::type_array<Load<T>, SaveLoad<T>, Serialization>;
        using mode = typename mode_array::template type<index>;
    };

public:
    template <class T> using SaveMode = typename SaveModeMeta<T>::mode;
    template <class T> using LoadMode = typename LoadModeMeta<T>::mode;

public:
    template <class T, typename = void> struct has_static_trait : std::false_type {};
    template <class T> struct has_static_trait<T, SFVOID(&T::__static_trait)> : std::true_type {};

    template <class T, typename = void> struct has_trait : std::false_type {};
    template <class T> struct has_trait<T, SFVOID(&T::__trait)> : std::true_type {};

public:
    template <class T> struct has_inner_trait
    {
        static constexpr bool value = has_static_trait<T>::value and has_trait<T>::value; // delay access
    };

private:
    template <typename From, typename To, typename = void> struct can_static_cast : std::false_type {};
    template <typename From, typename To>
    struct can_static_cast<From, To, SFVOID(static_cast<To>(std::declval<From>()))> : std::true_type {};

    template <typename From, typename To, typename = void> struct can_cast : std::false_type {};
    template <typename From, typename To>
    struct can_cast<From, To, SFVOID(std::declval<void(&)(To)>()(std::declval<From>()))> : std::true_type {};

    template <typename T, typename = void> struct is_returnable : std::false_type {};
    template <typename T>
    struct is_returnable<T, SFVOID(static_cast<T(*)()>(nullptr))> : std::true_type {};

public:
    template <class From, class To> struct is_cast_allowed
        : sf::meta::one<sf::meta::all<is_returnable<To>, can_cast<From, To>>,
                        sf::meta::is_same<void, From, To>> {};

    template <class From, class To> struct is_pointer_cast_allowed
        : sf::meta::one<is_cast_allowed<From*, To*>, sf::meta::is_void<From>> {};

public:
    template <class Base, class Derived> struct is_virtual_base_of
        : sf::meta::all<std::is_base_of<Base, Derived>,
                        sf::meta::negation<sf::meta::can_static_cast<Base*, Derived*>>> {};

public:
    using IOArchive = sf::core::IOArchive;
    using InstantiableTrait = sf::core::InstantiableTrait;

public:
    template <typename Archive, class T>
    static void save(Archive& archive, T& data)
    {
        SaveMode<T>(archive, data);
    }

    template <typename Archive, class T>
    static void load(Archive& archive, T& data)
    {
        LoadMode<T>(archive, data);
    }

    template <typename Base, class Archive, class Derived,
              SFREQUIRE(sf::meta::all<sf::meta::is_ioarchive<Archive>,
                                      std::is_base_of<Base, Derived>>::value)>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & cast<Base&>(object);
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>::value)>
    static InstantiableTrait::key_type trait(T& object)
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SFREQUIRE(has_inner_trait<T>::value)>
    static InstantiableTrait::key_type trait(T& object) noexcept
    {
        return object.__trait();
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>::value)>
    static InstantiableTrait::key_type static_trait() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SFREQUIRE(has_inner_trait<T>::value)>
    static constexpr InstantiableTrait::key_type static_trait() noexcept
    {
        return T::__static_trait();
    }

    template <class T, SFREQUIRE(not has_inner_trait<T>::value)>
    static InstantiableTrait::key_type trait() noexcept
    {
        constexpr auto trait_key = sf::core::InstantiableTraitKey<T>::key;

        static_assert(trait_key == InstantiableTrait::base_key,
            "Export instantiable trait is not allowed using typeid.");

        return static_trait<T>();
    }

    template <class T, SFREQUIRE(has_inner_trait<T>::value)>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr InstantiableTraitBase::key_type trait() noexcept
#else
    static InstantiableTrait::key_type trait() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto trait_key = sf::core::InstantiableTraitKey<T>::key;

        return trait_key == InstantiableTrait::base_key
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
