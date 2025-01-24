#ifndef SF_CORE_SERIALIZABLE_HPP
#define SF_CORE_SERIALIZABLE_HPP

#include <SerializationFixture/Core/ArchiveBase.hpp>

#include <SerializationFixture/Detail/Preprocessor.hpp>

#define SERIALIZATION(...) __VA_ARGS__

#define TEMPLATE_SERIALIZABLE_DECLARATION(object_template_header, ...) \
    SF_DEPAREN(object_template_header) struct xxsf<__VA_ARGS__> { \
        using S = __VA_ARGS__; \
        template <class ArchiveType> static void save(ArchiveType&, S&); \
        template <class ArchiveType> static void load(ArchiveType&, S&);

#define CONDITIONAL_SERIALIZABLE_DECLARATION(...) \
    template <typename S> struct xxsf<S, typename std::enable_if<__VA_ARGS__>::type> { \
        template <class ArchiveType> static void save(ArchiveType&, S&); \
        template <class ArchiveType> static void load(ArchiveType&, S&);

#define SERIALIZABLE_DECLARATION(...) \
    template <> struct xxsf<__VA_ARGS__> { \
        using S = __VA_ARGS__; \
        static void save(::sf::ioarchive_t&, S&); \
        static void load(::sf::ioarchive_t&, S&);


#define SERIALIZABLE_DECLARATION_INIT(...) \
    };


#define TEMPLATE_SERIALIZABLE(mode, object, object_template_header, ...) \
     SF_CONCAT(SF_TEMPLATE_SERIALIZABLE_, mode)(object, object_template_header, __VA_ARGS__)

#define CONDITIONAL_SERIALIZABLE(mode, object, ...) \
    SF_CONCAT(SF_CONDITIONAL_SERIALIZABLE_, mode)(object, __VA_ARGS__)

#define SERIALIZABLE(mode, object, ...) SF_CONCAT(SF_SERIALIZABLE_, mode)(object, __VA_ARGS__)

#define SERIALIZABLE_INIT(...) \
    }


// impl
#define SF_TEMPLATE_SERIALIZABLE(mode, object, object_template_header, ...) \
    SF_DEPAREN(object_template_header) \
    template <class ArchiveType> void xxsf<__VA_ARGS__>::mode(ArchiveType& archive, S& object) {

#define SF_TEMPLATE_SERIALIZABLE_save(object, object_template_header, ...) \
    SF_TEMPLATE_SERIALIZABLE(save, object, object_template_header, __VA_ARGS__)

#define SF_TEMPLATE_SERIALIZABLE_load(object, object_template_header, ...) \
    SF_TEMPLATE_SERIALIZABLE(load, object, object_template_header, __VA_ARGS__)

#define SF_TEMPLATE_SERIALIZABLE_saveload(object, object_template_header, ...) \
    SF_TEMPLATE_SERIALIZABLE_save(object, object_template_header, __VA_ARGS__) \
        SERIALIZATION(load(archive, object);) \
    SERIALIZABLE_INIT() \
    SF_TEMPLATE_SERIALIZABLE_load(object, object_template_header, __VA_ARGS__)

#define SF_CONDITIONAL_SERIALIZABLE(mode, object, ...) \
    template <typename S> \
    template <class ArchiveType> void xxsf<S, typename std::enable_if<__VA_ARGS__>::type>::mode(ArchiveType& archive, S& object) {

#define SF_CONDITIONAL_SERIALIZABLE_save(object, ...) SF_CONDITIONAL_SERIALIZABLE(save, object, __VA_ARGS__)
#define SF_CONDITIONAL_SERIALIZABLE_load(object, ...) SF_CONDITIONAL_SERIALIZABLE(load, object, __VA_ARGS__)

#define SF_CONDITIONAL_SERIALIZABLE_saveload(object, ...) \
    SF_CONDITIONAL_SERIALIZABLE_save(object, __VA_ARGS__) \
        SERIALIZATION(load(archive, object);) \
    SERIALIZABLE_INIT() \
    SF_CONDITIONAL_SERIALIZABLE_load(object, __VA_ARGS__) \

#define SF_SERIALIZABLE(mode, object, ...) \
    void xxsf<__VA_ARGS__>::mode(::sf::ioarchive_t& archive, S& object) {

#define SF_SERIALIZABLE_save(object, ...) SF_SERIALIZABLE(save, object, __VA_ARGS__)
#define SF_SERIALIZABLE_load(object, ...) SF_SERIALIZABLE(load, object, __VA_ARGS__)

#define SF_SERIALIZABLE_saveload(object, ...) \
    SF_SERIALIZABLE_save(object, __VA_ARGS__) \
        SERIALIZATION(load(archive, object);) \
    SERIALIZABLE_INIT() \
    SF_SERIALIZABLE_load(object, __VA_ARGS__)
// ~impl

// should be in global namespace
template <class SerializableType, typename enable = void> struct xxsf;

struct xxsf_cast_to_non_public_base
{
    template <class BaseType, class DerivedType>
    static BaseType& call(DerivedType& object)
    {
        return static_cast<BaseType&>(object);
    }
};

namespace sf
{

namespace meta
{

template <typename, typename enable = void> struct is_key : std::false_type {};
template <typename SerializableType>
struct is_key<SerializableType, std::void_t<decltype(&::xxsf<SerializableType>::key)>> : std::true_type {};

} // namespace meta

} // namespace sf

#endif // SF_CORE_SERIALIZABLE_HPP
