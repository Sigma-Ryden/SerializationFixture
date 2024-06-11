#ifndef SF_CORE_SERIALIZATION_HPP
#define SF_CORE_SERIALIZATION_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/InstantiableTraits.hpp>

#include <SF/Core/Hash.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp> // SF_WHEN, SF_VOID

#define SERIALIZATION(mode, ...)                                                                        \
    template <>                                                                                         \
    struct __sf::mode<__VA_ARGS__> {                                                                    \
        template <class Archive> mode(Archive&, __VA_ARGS__&);                                          \
    };                                                                                                  \
    template <class Archive>                                                                            \
    __sf::mode<__VA_ARGS__>::mode(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <class T>                                                                                  \
    struct __sf::mode<T, SF_WHEN(__VA_ARGS__)> {                                                        \
        template <class Archive> mode(Archive&, T&);                                                    \
    };                                                                                                  \
    template <class T> template <class Archive>                                                         \
    __sf::mode<T, SF_WHEN(__VA_ARGS__)>::mode(Archive& archive, T& self)

// Allow to hide implementationementation to translation unit, and declare interface in header
#define SERIALIZATION_INTERFACE(mode, ...)                                                              \
    template <>                                                                                         \
    struct __sf::mode<__VA_ARGS__> {                                                                    \
        mode(::sf::core::ioarchive_t&, __VA_ARGS__&);                                                   \
    };

#define SERIALIZATION_IMPLEMENTATION(mode, ...)                                                         \
    __sf::mode<__VA_ARGS__>::mode(::sf::core::ioarchive_t& archive, __VA_ARGS__& self)

// should be in global namespace
class __sf
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

        using mode_array = sf::meta::type_array<Save<T>, SaveLoad<T>, __sf>;
        using mode = typename mode_array::template type<index>;
    };

    template <class T>
    struct LoadModeMeta
    {
        static constexpr auto index = sf::meta::with<is_load_class<T>, is_saveload_class<T>>::value;

        using mode_array = sf::meta::type_array<Load<T>, SaveLoad<T>, __sf>;
        using mode = typename mode_array::template type<index>;
    };

public:
    template <class T> using SaveMode = typename SaveModeMeta<T>::mode;
    template <class T> using LoadMode = typename LoadModeMeta<T>::mode;

public:
    template <class T, typename = void> struct has_static_traits : std::false_type {};
    template <class T> struct has_static_traits<T, SF_VOID(&T::__static_traits)> : std::true_type {};

    template <class T, typename = void> struct has_traits : std::false_type {};
    template <class T> struct has_traits<T, SF_VOID(&T::__trait)> : std::true_type {};

public:
    template <class T> struct has_inner_traits
    {
        static constexpr bool value = has_static_traits<T>::value and has_traits<T>::value; // delay access
    };

private:
    template <typename From, typename To, typename = void> struct can_static_cast : std::false_type {};
    template <typename From, typename To>
    struct can_static_cast<From, To, SF_VOID(static_cast<To>(std::declval<From>()))> : std::true_type {};

    template <typename From, typename To, typename = void> struct can_cast : std::false_type {};
    template <typename From, typename To>
    struct can_cast<From, To, SF_VOID(std::declval<void(&)(To)>()(std::declval<From>()))> : std::true_type {};

    template <typename T, typename = void> struct is_returnable : std::false_type {};
    template <typename T>
    struct is_returnable<T, SF_VOID(static_cast<T(*)()>(nullptr))> : std::true_type {};

public:
    template <class From, class To> struct is_cast_allowed
        : sf::meta::one<sf::meta::all<is_returnable<To>, can_cast<From, To>>,
                        sf::meta::is_same<void, From, To>> {};

    template <class From, class To> struct is_pointer_cast_allowed
        : sf::meta::one<is_cast_allowed<From*, To*>, sf::meta::is_void<From>> {};

public:
    template <class Base, class Derived> struct is_virtual_base_of
        : sf::meta::all<std::is_base_of<Base, Derived>,
                        sf::meta::negation<can_static_cast<Base*, Derived*>>> {};

public:
    using ioarchive_t = sf::core::ioarchive_t;
    using instantiable_traits_t = sf::core::instantiable_traits_t;

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
              SF_REQUIRE(sf::meta::all<sf::meta::is_ioarchive<Archive>,
                                      std::is_base_of<Base, Derived>>::value)>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & static_cast<Base&>(object);
    }

    template <class T, SF_REQUIRE(not has_inner_traits<T>::value)>
    static instantiable_traits_t::key_type traits(T& object)
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SF_REQUIRE(has_inner_traits<T>::value)>
    static instantiable_traits_t::key_type traits(T& object) noexcept
    {
        return object.__trait();
    }

    template <class T, SF_REQUIRE(not has_inner_traits<T>::value)>
    static instantiable_traits_t::key_type static_traits() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SF_REQUIRE(has_inner_traits<T>::value)>
    static constexpr instantiable_traits_t::key_type static_traits() noexcept
    {
        return T::__static_traits();
    }

    template <class T, SF_REQUIRE(not has_inner_traits<T>::value)>
    static instantiable_traits_t::key_type traits() noexcept
    {
        constexpr auto traits_key = sf::core::instantiable_traits_key_t<T>::key;

        static_assert(traits_key == instantiable_traits_t::base_key,
            "Export instantiable traits is not allowed using typeid.");

        return static_traits<T>();
    }

    template <class T, SF_REQUIRE(has_inner_traits<T>::value)>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr InstantiableTraitsBase::key_type traits() noexcept
#else
    static instantiable_traits_t::key_type traits() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto traits_key = sf::core::instantiable_traits_key_t<T>::key;

        return traits_key == instantiable_traits_t::base_key
             ? static_traits<T>()
             : traits_key;
    }
};

#endif // SF_CORE_SERIALIZATION_HPP
