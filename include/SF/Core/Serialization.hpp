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
    struct xxsf_##mode<__VA_ARGS__> { template <class Archive> xxsf_##mode(Archive&, __VA_ARGS__&); };  \
    template <class Archive>                                                                            \
    xxsf_##mode<__VA_ARGS__>::xxsf_##mode(Archive& archive, __VA_ARGS__& self)

#define CONDITIONAL_SERIALIZATION(mode, ...)                                                            \
    template <class T>                                                                                  \
    struct xxsf_##mode<T, SF_WHEN(__VA_ARGS__)> { template <class Archive> xxsf_##mode(Archive&, T&); };\
    template <class T> template <class Archive>                                                         \
    xxsf_##mode<T, SF_WHEN(__VA_ARGS__)>::xxsf_##mode(Archive& archive, T& self)

// Allow to hide implementation to translation unit, and declare interface in header
#define SERIALIZATION_INTERFACE(mode, ...)                                                              \
    template <>                                                                                         \
    struct xxsf_##mode<__VA_ARGS__> { xxsf_##mode(::sf::core::ioarchive_t&, __VA_ARGS__&); };

#define SERIALIZATION_IMPLEMENTATION(mode, ...)                                                         \
    xxsf_##mode<__VA_ARGS__>::xxsf_##mode(::sf::core::ioarchive_t& archive, __VA_ARGS__& self)

// should be in global namespace

template <class T, typename enable = void>
struct xxsf_save;

template <class T, typename enable = void>
struct xxsf_load;

template <class T, typename enable = void>
struct xxsf_saveload;

class xxsf
{
public:
    using key_type = ::xxsf_traits<void>::key_type;

public:
    template <class T> struct is_save_class : sf::meta::is_complete<::xxsf_save<T>> {};
    template <class T> struct is_load_class : sf::meta::is_complete<::xxsf_load<T>> {};
    template <class T> struct is_saveload_class : sf::meta::is_complete<::xxsf_saveload<T>> {};

public:
    template <class T> struct has_save_mode : sf::meta::one<is_save_class<T>, is_saveload_class<T>> {};
    template <class T> struct has_load_mode : sf::meta::one<is_load_class<T>, is_saveload_class<T>> {};

private:
    template <class T>
    struct SaveModeMeta
    {
        static constexpr auto index = sf::meta::with<0, is_save_class<T>, is_saveload_class<T>>::value;

        using mode_array = std::tuple<::xxsf_save<T>, ::xxsf_saveload<T>>;
        using mode = typename std::tuple_element<index, mode_array>::type;
    };

    template <class T>
    struct LoadModeMeta
    {
        static constexpr auto index = sf::meta::with<0, is_load_class<T>, is_saveload_class<T>>::value;

        using mode_array = std::tuple<::xxsf_load<T>, ::xxsf_saveload<T>>;
        using mode = typename std::tuple_element<index, mode_array>::type;
    };

public:
    template <class T, typename = void> struct has_static_traits : std::false_type {};
    template <class T> struct has_static_traits<T, SF_VOID(&T::xxstatic_traits)> : std::true_type {};

    template <class T, typename = void> struct has_traits : std::false_type {};
    template <class T> struct has_traits<T, SF_VOID(&T::xxtrait)> : std::true_type {};

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
    template <typename Archive, class T>
    static void save(Archive& archive, T& data)
    {
        typename SaveModeMeta<T>::mode(archive, data);
    }

    template <typename Archive, class T>
    static void load(Archive& archive, T& data)
    {
        typename LoadModeMeta<T>::mode(archive, data);
    }

    template <typename Base, class Archive, class Derived,
              SF_REQUIRE(sf::meta::all<sf::meta::is_ioarchive<Archive>,
                                       std::is_base_of<Base, Derived>>::value)>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & static_cast<Base&>(object);
    }

    template <class T, SF_REQUIRE(not has_inner_traits<T>::value)>
    static key_type traits(T& object)
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SF_REQUIRE(has_inner_traits<T>::value)>
    static key_type traits(T& object) noexcept
    {
        return object.xxtrait();
    }

    template <class T, SF_REQUIRE(not has_inner_traits<T>::value)>
    static key_type static_traits() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SF_REQUIRE(has_inner_traits<T>::value)>
    static constexpr key_type static_traits() noexcept
    {
        return T::xxstatic_traits();
    }

    template <class T, SF_REQUIRE(not has_inner_traits<T>::value)>
    static key_type traits() noexcept
    {
        constexpr auto traits_key = ::xxsf_traits<T>::key;

        static_assert(traits_key == ::xxsf_traits<void>::base_key,
            "Export instantiable traits is not allowed using typeid.");

        return static_traits<T>();
    }

    template <class T, SF_REQUIRE(has_inner_traits<T>::value)>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr InstantiableTraitsBase::key_type traits() noexcept
#else
    static key_type traits() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto traits_key = ::xxsf_traits<T>::key;

        return traits_key == ::xxsf_traits<void>::base_key
             ? static_traits<T>()
             : traits_key;
    }
};

#endif // SF_CORE_SERIALIZATION_HPP
