#ifndef SIFAR_POLYMORPHIC_HPP
#define SIFAR_POLYMORPHIC_HPP

#include <Sifar/Access.hpp> // SERIALIZATION_ACCESS

#include <Sifar/FactoryTable.hpp>
// _CLONEABLE_IMPORT, _CLONEABLE_DECLARATION, _CLONEABLE_IMPLEMENTATION,
// _CLONEABLE_TEMPLATE_DECLARATION, _CLONEABLE_TEMPLATE_IMPLEMENTATION

#include <Sifar/Serializable.hpp> // SERIALIZATION_SAVE_LOAD

#include <Sifar/PolymorphicArchive.hpp>

// Private macro defs
#define _DYNAMIC_SAVE_LOAD_DECLARATION(...)                                                             \
    private:                                                                                            \
    virtual void save(Archive& archive) override;                                                       \
    virtual void load(Archive& archive) override;                                                       \
    public:

#define _POLYMORPHIC_ARCHIVE_CALL(function_name)                                                        \
    { ::sifar::dynamic::PolymorphicArchive::function_name(archive, *this); }

#define _DYNAMIC_SAVE_LOAD_IMPLEMENTATION_WITH(prefix, ...)                                             \
    prefix void __VA_ARGS__::save(Archive& archive) _POLYMORPHIC_ARCHIVE_CALL(save)                     \
    prefix void __VA_ARGS__::load(Archive& archive) _POLYMORPHIC_ARCHIVE_CALL(load)

#define _DYNAMIC_SAVE_LOAD_IMPLEMENTATION(...)                                                          \
    _DYNAMIC_SAVE_LOAD_IMPLEMENTATION_WITH(, __VA_ARGS__)

#define _DYNAMIC_SAVE_LOAD_TEMPLATE_IMPLEMENTATION(...)                                                 \
    _DYNAMIC_SAVE_LOAD_IMPLEMENTATION_WITH(template <>, __VA_ARGS__)

// Public macro defs
#define IMPORT_SERIALIZABLE_POLYMORPHIC(...)                                                            \
    public ::sifar::dynamic::Polymorphic

#define SERIALIZABLE_POLYMORPHIC(...)                                                                   \
    SERIALIZABLE()                                                                                      \
    _CLONEABLE_DECLARATION()                                                                            \
    _FACTORY_TABLE_UPDATER_DECLARATION()                                                                \
    _DYNAMIC_SAVE_LOAD_DECLARATION()

#define SERIALIZABLE_POLYMORPHIC_TEMPLATE(...)                                                          \
    SERIALIZABLE()                                                                                      \
    _CLONEABLE_TEMPLATE_DECLARATION(__VA_ARGS__)                                                        \
    _FACTORY_TABLE_UPDATER_DECLARATION()                                                                \
    _DYNAMIC_SAVE_LOAD_DECLARATION()

#define EXPORT_SERIALIZABLE_POLYMORPHIC(...)                                                            \
    _CLONEABLE_IMPLEMENTATION(__VA_ARGS__)                                                              \
    _FACTORY_TABLE_UPDATER_IMPLEMENTATION(__VA_ARGS__)                                                  \
    _DYNAMIC_SAVE_LOAD_IMPLEMENTATION(__VA_ARGS__)

#define EXPORT_SERIALIZABLE_POLYMORPHIC_TEMPLATE(...)                                                   \
    _CLONEABLE_TEMPLATE_IMPLEMENTATION(__VA_ARGS__)                                                     \
    _FACTORY_TABLE_UPDATER_TEMPLATE_IMPLEMENTATION(__VA_ARGS__)                                         \
    _DYNAMIC_SAVE_LOAD_TEMPLATE_IMPLEMENTATION(__VA_ARGS__)

// Special serialization case (export only for translation unit scope)
#define SAVE_LOAD_SERIALIZABLE_POLYMORPHIC(...)                                                         \
    EXPORT_SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)                                                        \
    SAVE_LOAD_SERIALIZABLE(__VA_ARGS__)

#ifdef SIFAR_SMART
    // Additional macro defs
    #define POLYMORPHIC(...) SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)
    #define POLYMORPHIC_TEMPLATE(...) SERIALIZABLE_POLYMORPHIC_TEMPLATE(__VA_ARGS__)
    #define EXPORT_POLYMORPHIC(...) EXPORT_SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)
    #define EXPORT_POLYMORPHIC_TEMPLATE(...) EXPORT_SERIALIZABLE_POLYMORPHIC_TEMPLATE(__VA_ARGS__)
    #define IMPORT_POLYMORPHIC(...) IMPORT_SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)
    #define SERIALIZATION_POLYMORPHIC(...) SAVE_LOAD_SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)
#endif

namespace sifar
{

namespace dynamic
{

struct Polymorphic : public Cloneable
{
    using Archive = ::sifar::core::ArchiveBase;

private:
    virtual void save(Archive& archive) = 0;
    virtual void load(Archive& archive) = 0;
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_POLYMORPHIC_HPP
