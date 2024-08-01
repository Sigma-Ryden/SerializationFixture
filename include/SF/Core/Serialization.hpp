#ifndef SF_CORE_SERIALIZATION_HPP
#define SF_CORE_SERIALIZATION_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/InstantiableTraits.hpp>

#include <SF/Core/Hash.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>
#include <SF/Detail/Preprocessor.hpp>

#define CONDITIONAL_SERIALIZATION_save(object, ...)                                                                     \
    template <class T>                                                                                                  \
    struct xxsf_save<T, typename std::enable_if<__VA_ARGS__>::type> { xxsf_save(::sf::core::ioarchive_t&, T&); };        \
    template <class T>                                                                                                  \
    xxsf_save<T, typename std::enable_if<__VA_ARGS__>::type>::xxsf_save(::sf::core::ioarchive_t& archive, T& object)

#define CONDITIONAL_SERIALIZATION_load(object, ...)                                                                     \
    template <class T>                                                                                                  \
    struct xxsf_load<T, typename std::enable_if<__VA_ARGS__>::type> { xxsf_load(::sf::core::ioarchive_t&, T&); };        \
    template <class T>                                                                                                  \
    xxsf_load<T, typename std::enable_if<__VA_ARGS__>::type>::xxsf_load(::sf::core::ioarchive_t& archive, T& object)

#define CONDITIONAL_SERIALIZATION_saveload(object, ...)                                                                 \
    template <class T>                                                                                                  \
    struct xxsf_saveload<T, typename std::enable_if<__VA_ARGS__>::type> { xxsf_saveload(::sf::core::ioarchive_t&, T&); };\
    template <class T> struct xxsf_save<T, typename std::enable_if<__VA_ARGS__>::type> {\
        xxsf_save(::sf::core::ioarchive_t& archive, T& object) { xxsf_saveload<T>(archive, object); }\
     };           \
    template <class T> struct xxsf_load<T, typename std::enable_if<__VA_ARGS__>::type> {           \
        xxsf_load(::sf::core::ioarchive_t& archive, T& object) { xxsf_saveload<T>(archive, object); }\
     };           \
    template <class T>                                                                                                  \
    xxsf_saveload<T, typename std::enable_if<__VA_ARGS__>::type>::xxsf_saveload(::sf::core::ioarchive_t& archive, T& object)

#define CONDITIONAL_SERIALIZATION(mode, object, ...) SF_CONCAT(CONDITIONAL_SERIALIZATION_, mode)(object, __VA_ARGS__)

// Allow to hide implementation to translation unit, and declare interface in header
#define SERIALIZATION_INTERFACE_save(object, ...)                                                                       \
    template <> struct xxsf_save<__VA_ARGS__> { xxsf_save(::sf::core::ioarchive_t&, __VA_ARGS__& object); };

#define SERIALIZATION_INTERFACE_load(object, ...)                                                                       \
    template <> struct xxsf_load<__VA_ARGS__> { xxsf_load(::sf::core::ioarchive_t&, __VA_ARGS__& object); };

#define SERIALIZATION_INTERFACE_saveload(object, ...)                                                                   \
    template <> struct xxsf_saveload<__VA_ARGS__> { xxsf_saveload(::sf::core::ioarchive_t&, __VA_ARGS__& object); };    \
    template <> struct xxsf_save<__VA_ARGS__> { xxsf_save(::sf::core::ioarchive_t&, __VA_ARGS__& object); }; \
    template <> struct xxsf_load<__VA_ARGS__> : xxsf_saveload<__VA_ARGS__> {};

#define SERIALIZATION_INTERFACE(mode, object, ...) SF_CONCAT(SERIALIZATION_INTERFACE_, mode)(object, __VA_ARGS__)

#define SERIALIZATION_IMPLEMENTATION_save( object, ...)                                                                 \
    xxsf_save<__VA_ARGS__>::xxsf_save(::sf::core::ioarchive_t& archive, __VA_ARGS__& object)

#define SERIALIZATION_IMPLEMENTATION_load(object, ...)                                                                  \
    xxsf_load<__VA_ARGS__>::xxsf_load(::sf::core::ioarchive_t& archive, __VA_ARGS__& object)

#define SERIALIZATION_IMPLEMENTATION_saveload(object, ...)                                                              \
    xxsf_saveload<__VA_ARGS__>::xxsf_saveload(::sf::core::ioarchive_t& archive, __VA_ARGS__& object)

#define SERIALIZATION_IMPLEMENTATION(mode, object, ...) SF_CONCAT(SERIALIZATION_IMPLEMENTATION_, mode)(object, __VA_ARGS__)

#define SERIALIZATION(mode, object, ...) \
    SERIALIZATION_INTERFACE(mode, object, __VA_ARGS__) \
    SERIALIZATION_IMPLEMENTATION(mode, object, __VA_ARGS__)

// should be in global namespace
template <class T, typename enable = void> struct xxsf_save;
template <class T, typename enable = void> struct xxsf_load;
template <class T, typename enable = void> struct xxsf_saveload;

class xxsf
{
public:
    using trait_type = ::xxsf_instantiable_traits<void>::key_type;

public:
    template <class T, typename = void> struct is_has_static_traits : std::false_type {};
    template <class T>
    struct is_has_static_traits<T, ::sf::meta::void_t<decltype(&T::xxstatic_traits)>> : std::true_type {};

    template <class T, typename = void> struct is_has_traits : std::false_type {};
    template <class T>
    struct is_has_traits<T, ::sf::meta::void_t<decltype(&T::xxtrait)>> : std::true_type {};

public:
    template <class T> struct is_has_inner_traits
    {
        static constexpr bool value = is_has_static_traits<T>::value and is_has_traits<T>::value; // delay access
    };

public:
    template <typename Base, class Archive, class Derived>
    static void serialize_base(Archive& archive, Derived& object)
    {
        archive & static_cast<Base&>(object);
    }

public:
    template <class T, SF_REQUIRE(not is_has_inner_traits<T>::value)>
    static trait_type traits(T& object)
    {
    #ifdef SF_EXTERN_RUNTIME_TRAIT
        return SF_EXTERN_RUNTIME_TRAIT(object);
    #else
        return SF_TYPE_HASH(typeid(object));
    #endif // SF_EXTERN_RUNTIME_TRAIT
    }

    template <class T, SF_REQUIRE(is_has_inner_traits<T>::value)>
    static trait_type traits(T& object) noexcept
    {
        return object.xxtrait();
    }

    template <class T, SF_REQUIRE(not is_has_inner_traits<T>::value)>
    static trait_type static_traits() noexcept
    {
    #ifdef SF_EXTERN_TRAIT
        return SF_EXTERN_TRAIT(T);
    #else
        return SF_TYPE_HASH(typeid(T));
    #endif // SF_EXTERN_TRAIT
    }

    template <class T, SF_REQUIRE(is_has_inner_traits<T>::value)>
    static constexpr trait_type static_traits() noexcept
    {
        return T::xxstatic_traits();
    }

    template <class T, SF_REQUIRE(not is_has_inner_traits<T>::value)>
    static trait_type traits() noexcept
    {
        static_assert(::xxsf_instantiable_traits<T>::key == ::xxsf_instantiable_traits<void>::base_key,
            "Export instantiable traits is not allowed using typeid.");

        return static_traits<T>();
    }

    template <class T, SF_REQUIRE(is_has_inner_traits<T>::value)>
#ifdef SF_EXPORT_INSTANTIABLE_DISABLE
    static constexpr InstantiableTraitsBase::key_type traits() noexcept
#else
    static trait_type traits() noexcept
#endif // SF_EXPORT_INSTANTIABLE_DISABLE
    {
        constexpr auto traits_key = ::xxsf_instantiable_traits<T>::key;

        return traits_key == ::xxsf_instantiable_traits<void>::base_key
             ? static_traits<T>()
             : traits_key;
    }
};

#endif // SF_CORE_SERIALIZATION_HPP
