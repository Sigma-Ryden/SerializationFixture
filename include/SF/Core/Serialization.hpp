#ifndef SF_CORE_SERIALIZATION_HPP
#define SF_CORE_SERIALIZATION_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/InstantiableTraits.hpp>

#include <SF/Core/Hash.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>
#include <SF/Detail/Preprocessor.hpp>

#define SERIALIZATION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)((template <>), (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType)) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)((), (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType), object)

#define TEMPLATE_SERIALIZATION(mode, object, object_template_header, ...) \
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)(object_template_header, (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType)) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)(object_template_header, (__VA_ARGS__), (template <class ArchiveType>), (ArchiveType), object)

#define CONDITIONAL_SERIALIZATION(mode, object, ...) \
    SF_CONCAT(SF_CONDITIONAL_SERIALIZATION_DECLARATION_, mode)((__VA_ARGS__), (template <class ArchiveType>), (ArchiveType)) \
    SF_CONCAT(SF_CONDITIONAL_SERIALIZATION_DEFINITION_, mode)((__VA_ARGS__), (template <class ArchiveType>), (ArchiveType), object)

// allow to hide implementation to translation unit, and declare interface in header
#define SERIALIZATION_DECLARATION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)((template <>), (__VA_ARGS__), (), (::sf::core::ioarchive_t))

#define SERIALIZATION_DEFINITION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)((), (__VA_ARGS__), (), (::sf::core::ioarchive_t), object)

// Impl
#define SF_SERIALIZATION_DECLARATION(mode, object_template_header, object_type, archive_template_header, archive_type) \
    SF_DEPAREN(object_template_header) \
    struct xxsf_##mode<SF_DEPAREN(object_type)> { \
        SF_DEPAREN(archive_template_header) \
        xxsf_##mode(SF_DEPAREN(archive_type)&, SF_DEPAREN(object_type)&); \
    };

#define SF_SERIALIZATION_DECLARATION_save(object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(save, object_template_header, object_type, archive_template_header, archive_type)

#define SF_SERIALIZATION_DECLARATION_load(object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(load, object_template_header, object_type, archive_template_header, archive_type)

#define SF_SERIALIZATION_DECLARATION_saveload(object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(save, object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(load, object_template_header, object_type, archive_template_header, archive_type) \
    SF_SERIALIZATION_DECLARATION(saveload, object_template_header, object_type, archive_template_header, archive_type)

#define SF_SERIALIZATION_DEFINITION(mode, object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_DEPAREN(object_template_header) \
    SF_DEPAREN(archive_template_header) \
    xxsf_##mode<SF_DEPAREN(object_type)>::xxsf_##mode(SF_DEPAREN(archive_type)& archive, SF_DEPAREN(object_type)& object)

#define SF_SERIALIZATION_DEFINITION_save(object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_SERIALIZATION_DEFINITION(save, object_template_header, object_type, archive_template_header, archive_type, object)

#define SF_SERIALIZATION_DEFINITION_load(object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_SERIALIZATION_DEFINITION(load, object_template_header, object_type, archive_template_header, archive_type, object)

#define SF_SERIALIZATION_DEFINITION_saveload(object_template_header, object_type, archive_template_header, archive_type, object) \
    SF_SERIALIZATION_DEFINITION(save, object_template_header, object_type, archive_template_header, archive_type, object) \
    { xxsf_saveload<SF_DEPAREN(object_type)>(archive, object); } \
    SF_SERIALIZATION_DEFINITION(load, object_template_header, object_type, archive_template_header, archive_type, object) \
    { xxsf_saveload<SF_DEPAREN(object_type)>(archive, object); }\
    SF_SERIALIZATION_DEFINITION(saveload, object_template_header, object_type, archive_template_header, archive_type, object)


#define SF_CONDITIONAL_SERIALIZATION_DECLARATION(mode, object_type_condition, archive_template_header, archive_type) \
    template <typename T> \
    struct xxsf_##mode<T, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type> { \
        SF_DEPAREN(archive_template_header) \
        xxsf_##mode(SF_DEPAREN(archive_type)&, T&); \
    };

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION_save(object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(save, object_type_condition, archive_template_header, archive_type)

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION_load(object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(load, object_type_condition, archive_template_header, archive_type)

#define SF_CONDITIONAL_SERIALIZATION_DECLARATION_saveload(object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(save, object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(load, object_type_condition, archive_template_header, archive_type) \
    SF_CONDITIONAL_SERIALIZATION_DECLARATION(saveload, object_type_condition, archive_template_header, archive_type)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION(mode, object_type_condition, archive_template_header, archive_type, object) \
    template <typename T> \
    SF_DEPAREN(archive_template_header) \
    xxsf_##mode<T, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type>::xxsf_##mode(SF_DEPAREN(archive_type)& archive, T& object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_save(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_load(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_saveload(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<T>(archive, object); } \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<T>(archive, object); }\
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(saveload, object_type_condition, archive_template_header, archive_type, object)
// ~Impl

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
    template <typename Base, class ArchiveType, class Derived>
    static void serialize_base(ArchiveType& archive, Derived& object)
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
