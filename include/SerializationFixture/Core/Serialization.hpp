#ifndef SF_CORE_SERIALIZATION_HPP
#define SF_CORE_SERIALIZATION_HPP

#include <SerializationFixture/Core/ArchiveBase.hpp>

#include <SerializationFixture/Detail/Preprocessor.hpp>

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
     SF_CONCAT(SF_SERIALIZATION_DECLARATION_, mode)((template <>), (__VA_ARGS__), (), (::sf::ioarchive_t))

#define SERIALIZATION_DEFINITION(mode, object, ...) \
     SF_CONCAT(SF_SERIALIZATION_DEFINITION_, mode)((), (__VA_ARGS__), (), (::sf::ioarchive_t), object)

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
    template <typename S> \
    struct xxsf_##mode<S, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type> { \
        SF_DEPAREN(archive_template_header) \
        xxsf_##mode(SF_DEPAREN(archive_type)&, S&); \
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
    template <typename S> \
    SF_DEPAREN(archive_template_header) \
    xxsf_##mode<S, typename std::enable_if<SF_DEPAREN(object_type_condition)>::type>::xxsf_##mode(SF_DEPAREN(archive_type)& archive, S& object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_save(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_load(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object)

#define SF_CONDITIONAL_SERIALIZATION_DEFINITION_saveload(object_type_condition, archive_template_header, archive_type, object) \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(save, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<S>(archive, object); } \
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(load, object_type_condition, archive_template_header, archive_type, object) \
    { ::xxsf_saveload<S>(archive, object); }\
    SF_CONDITIONAL_SERIALIZATION_DEFINITION(saveload, object_type_condition, archive_template_header, archive_type, object)
// ~Impl

// should be in global namespace
template <class SerializableType, typename enable = void> struct xxsf_save;
template <class SerializableType, typename enable = void> struct xxsf_load;
template <class SerializableType, typename enable = void> struct xxsf_saveload;

struct xxsf_cast_to_non_public_base
{
    template <class BaseType, class DerivedType>
    static BaseType& call(DerivedType& object)
    {
        return static_cast<BaseType&>(object);
    }
};

#endif // SF_CORE_SERIALIZATION_HPP
