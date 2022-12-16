#ifndef SIRAF_POLYMORPHIC_HPP
#define SIRAF_POLYMORPHIC_HPP

#include <Siraf/Access.hpp> // SERIALIZABLE
#include <Siraf/Dynamic/FactoryTable.hpp> // _CLONEABLE_BODY

#include <Siraf/Dynamic/PolymorphicArchive.hpp>

#define _POLYMORPHIC_ARCHIVE_CALL(function_name)                                                        \
    { ::siraf::dynamic::PolymorphicArchive::function_name(archive, *this); }

#define _DYNAMIC_SAVE_LOAD_IMPLEMENTATION(...)                                                          \
    private:                                                                                            \
    virtual void __save(Archive& archive) override _POLYMORPHIC_ARCHIVE_CALL(save)                      \
    virtual void __load(Archive& archive) override _POLYMORPHIC_ARCHIVE_CALL(load)                      \
    public:

#define _POLYMORPHIC_BASE_BODY(...)                                                                     \
    _CLONEABLE_BODY(__VA_ARGS__)                                                                        \
    _DYNAMIC_SAVE_LOAD_IMPLEMENTATION()

#ifndef POLYMORPHIC_BASE
    #define POLYMORPHIC_BASE(...)                                                                       \
        public ::siraf::dynamic::Polymorphic
#endif // POLYMORPHIC_BASE

#ifndef SERIALIZABLE_POLYMORPHIC
    #define SERIALIZABLE_POLYMORPHIC(...)                                                               \
        SERIALIZABLE()                                                                                  \
        _POLYMORPHIC_BASE_BODY(__VA_ARGS__)
#endif // SERIALIZABLE_POLYMORPHIC

#ifdef SIRAF_SMART
    // Additional macro defs
    #define POLYMORPHIC(...) SERIALIZABLE_POLYMORPHIC(__VA_ARGS__)
#endif // SIRAF_SMART

namespace siraf
{

namespace dynamic
{

class Polymorphic : public Cloneable
{
public:
    using Archive = ::siraf::core::ArchiveBase;

private:
    virtual void __save(Archive& archive) = 0;
    virtual void __load(Archive& archive) = 0;
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_POLYMORPHIC_HPP
