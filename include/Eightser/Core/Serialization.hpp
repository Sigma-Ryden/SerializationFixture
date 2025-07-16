#ifndef EIGHTSER_CORE_SERIALIZABLE_HPP
#define EIGHTSER_CORE_SERIALIZABLE_HPP

#include <Eightser/Core/ArchiveBase.hpp>

#include <Eightser/Detail/Preprocessor.hpp>

#ifdef EIGHTSER_EXTENDED_SERIALIZATION
    #define SERIALIZATION(...) EIGHTSER_SERIALIZATION(bin, __VA_ARGS__)
    #define SERIALIZATION_XML(...) EIGHTSER_SERIALIZATION(xml, __VA_ARGS__)
    #define SERIALIZATION_JSON(...) EIGHTSER_SERIALIZATION(json, __VA_ARGS__)
#else
    #define SERIALIZATION(...) __VA_ARGS__
#endif // EIGHTSER_MULTY_SERIALIZATION

#define TEMPLATE_SERIALIZABLE_DECLARATION(object_template_header, ...) \
    EIGHTSER_DEPAREN(object_template_header) struct xxeightser<__VA_ARGS__> { \
        using S = __VA_ARGS__; \
        template <class ArchiveType> static void save(ArchiveType&, S&); \
        template <class ArchiveType> static void load(ArchiveType&, S&);

#define CONDITIONAL_SERIALIZABLE_DECLARATION(...) \
    template <typename S> struct xxeightser<S, typename std::enable_if<__VA_ARGS__>::type> { \
        template <class ArchiveType> static void save(ArchiveType&, S&); \
        template <class ArchiveType> static void load(ArchiveType&, S&);

#define SERIALIZABLE_DECLARATION(...) \
    template <> struct xxeightser<__VA_ARGS__> { \
        using S = __VA_ARGS__; \
        static void save(::eightser::ioarchive_t&, S&); \
        static void load(::eightser::ioarchive_t&, S&);


#define SERIALIZABLE_DECLARATION_INIT(...) \
    };


#define TEMPLATE_SERIALIZABLE(mode, object, object_template_header, ...) \
     EIGHTSER_CONCAT(EIGHTSER_TEMPLATE_SERIALIZABLE_, mode)(object, object_template_header, __VA_ARGS__)

#define CONDITIONAL_SERIALIZABLE(mode, object, ...) \
    EIGHTSER_CONCAT(EIGHTSER_CONDITIONAL_SERIALIZABLE_, mode)(object, __VA_ARGS__)

#define SERIALIZABLE(mode, object, ...) EIGHTSER_CONCAT(EIGHTSER_SERIALIZABLE_, mode)(object, __VA_ARGS__)

#define SERIALIZABLE_INIT(...) \
    }


// impl
#ifdef EIGHTSER_EXTENDED_SERIALIZATION
    #define EIGHTSER_SERIALIZATION(archive_type, ...) if (archive.type == ::eightser::archive_type) { __VA_ARGS__ return; }
#endif // EIGHTSER_EXTENDED_SERIALIZATION

#define EIGHTSER_TEMPLATE_SERIALIZABLE(mode, object, object_template_header, ...) \
    EIGHTSER_DEPAREN(object_template_header) \
    template <class ArchiveType> void xxeightser<__VA_ARGS__>::mode(ArchiveType& archive, S& object) {

#define EIGHTSER_TEMPLATE_SERIALIZABLE_save(object, object_template_header, ...) \
    EIGHTSER_TEMPLATE_SERIALIZABLE(save, object, object_template_header, __VA_ARGS__)

#define EIGHTSER_TEMPLATE_SERIALIZABLE_load(object, object_template_header, ...) \
    EIGHTSER_TEMPLATE_SERIALIZABLE(load, object, object_template_header, __VA_ARGS__)

#define EIGHTSER_TEMPLATE_SERIALIZABLE_saveload(object, object_template_header, ...) \
    EIGHTSER_TEMPLATE_SERIALIZABLE_save(object, object_template_header, __VA_ARGS__) \
        SERIALIZATION(load(archive, object);) \
    SERIALIZABLE_INIT() \
    EIGHTSER_TEMPLATE_SERIALIZABLE_load(object, object_template_header, __VA_ARGS__)

#define EIGHTSER_CONDITIONAL_SERIALIZABLE(mode, object, ...) \
    template <typename S> \
    template <class ArchiveType> void xxeightser<S, typename std::enable_if<__VA_ARGS__>::type>::mode(ArchiveType& archive, S& object) {

#define EIGHTSER_CONDITIONAL_SERIALIZABLE_save(object, ...) EIGHTSER_CONDITIONAL_SERIALIZABLE(save, object, __VA_ARGS__)
#define EIGHTSER_CONDITIONAL_SERIALIZABLE_load(object, ...) EIGHTSER_CONDITIONAL_SERIALIZABLE(load, object, __VA_ARGS__)

#define EIGHTSER_CONDITIONAL_SERIALIZABLE_saveload(object, ...) \
    EIGHTSER_CONDITIONAL_SERIALIZABLE_save(object, __VA_ARGS__) \
        SERIALIZATION(load(archive, object);) \
    SERIALIZABLE_INIT() \
    EIGHTSER_CONDITIONAL_SERIALIZABLE_load(object, __VA_ARGS__) \

#define EIGHTSER_SERIALIZABLE(mode, object, ...) \
    void xxeightser<__VA_ARGS__>::mode(::eightser::ioarchive_t& archive, S& object) {

#define EIGHTSER_SERIALIZABLE_save(object, ...) EIGHTSER_SERIALIZABLE(save, object, __VA_ARGS__)
#define EIGHTSER_SERIALIZABLE_load(object, ...) EIGHTSER_SERIALIZABLE(load, object, __VA_ARGS__)

#define EIGHTSER_SERIALIZABLE_saveload(object, ...) \
    EIGHTSER_SERIALIZABLE_save(object, __VA_ARGS__) \
        SERIALIZATION(load(archive, object);) \
    SERIALIZABLE_INIT() \
    EIGHTSER_SERIALIZABLE_load(object, __VA_ARGS__)
// ~impl

// should be in global namespace
template <class SerializableType, typename enable = void> struct xxeightser;

struct xxeightser_cast_to_non_public_base
{
    template <class BaseType, class DerivedType>
    static BaseType& call(DerivedType& object)
    {
        return static_cast<BaseType&>(object);
    }
};

namespace eightser
{

namespace meta
{

template <typename, typename enable = void> struct is_key : std::false_type {};
template <typename SerializableType>
struct is_key<SerializableType, std::void_t<decltype(&::xxeightser<SerializableType>::key)>> : std::true_type {};

} // namespace meta

} // namespace eightser

#endif // EIGHTSER_CORE_SERIALIZABLE_HPP
