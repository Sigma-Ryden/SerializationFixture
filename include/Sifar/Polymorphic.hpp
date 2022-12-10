#ifndef SIFAR_POLYMORPHIC_HPP
#define SIFAR_POLYMORPHIC_HPP

#include <Sifar/Access.hpp> // SERIALIZABLE
#include <Sifar/FactoryTable.hpp> // CLONEABLE_BODY

#include <Sifar/PolymorphicArchive.hpp>

#define _POLYMORPHIC_ARCHIVE_CALL(function_name)                                                        \
    { ::sifar::dynamic::PolymorphicArchive::function_name(archive, *this); }

#define _DYNAMIC_SAVE_LOAD_IMPLEMENTATION(...)                                                          \
    private:                                                                                            \
    virtual void save(Archive& archive) override _POLYMORPHIC_ARCHIVE_CALL(save)                        \
    virtual void load(Archive& archive) override _POLYMORPHIC_ARCHIVE_CALL(load)

#define _POLYMORPHIC_BASE_BODY(...)                                                                     \
    CLONEABLE_BODY(__VA_ARGS__)                                                                         \
    _DYNAMIC_SAVE_LOAD_IMPLEMENTATION()

#define POLYMORPHIC_BASE(...)                                                                           \
    public ::sifar::dynamic::Polymorphic

#define SERIALIZABLE_POLYMORPHIC(...)                                                                   \
    SERIALIZABLE()                                                                                      \
    _POLYMORPHIC_BASE_BODY(__VA_ARGS__)

#ifdef SIFAR_SMART
    // Additional macro defs
    #define POLYMORPHIC(...) SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)
#endif // SIFAR_SMART

namespace sifar
{

namespace dynamic
{

class Polymorphic : public Cloneable
{
public:
    using Archive = ::sifar::core::ArchiveBase;

private:
    virtual void save(Archive& archive) = 0;
    virtual void load(Archive& archive) = 0;
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_POLYMORPHIC_HPP
