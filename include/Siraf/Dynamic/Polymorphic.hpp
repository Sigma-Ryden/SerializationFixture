#ifndef SIRAF_DYNAMIC_POLYMORPHIC_HPP
#define SIRAF_DYNAMIC_POLYMORPHIC_HPP

#include <Siraf/Core/Serialization.hpp> // SERIALIZATION_ACCESS
#include <Siraf/Dynamic/FactoryTable.hpp> // CLONEABLE_BODY

#include <Siraf/Dynamic/PolymorphicArchive.hpp>

#define _POLYMORPHIC_ALIASE_IMPLEMENTATION(...)                                                         \
    using clone_type = siraf::dynamic::FactoryTableBase::clone_type;                                    \
    using key_type = siraf::dynamic::FactoryTableBase::key_type;

#define _POLYMORPHIC_INTERFACE_IMPLEMENTATION(function_name)                                            \
    void __##function_name(siraf::core::ArchiveBase& archive)                                           \
    { siraf::dynamic::PolymorphicArchive::function_name(archive, *this); }

#define _POLYMORPHIC_IMPLEMENTATION(...)                                                                \
    _POLYMORPHIC_INTERFACE_IMPLEMENTATION(save)                                                         \
    _POLYMORPHIC_INTERFACE_IMPLEMENTATION(load)

#define POLYMORPHIC_BODY(...)                                                                           \
    private:                                                                                            \
    _POLYMORPHIC_ALIASE_IMPLEMENTATION()                                                                \
    _POLYMORPHIC_IMPLEMENTATION(__VA_ARGS__)                                                            \
    CLONEABLE_BODY(__VA_ARGS__)                                                                         \
    public:

#ifndef POLYMORPHIC
    #define POLYMORPHIC(...)                                                                            \
        public siraf::dynamic::Polymorphic
#endif // POLYMORPHIC

#ifndef SERIALIZABLE
    #define SERIALIZABLE(...)                                                                           \
        SERIALIZATION_ACCESS(__VA_ARGS__)                                                               \
        POLYMORPHIC_BODY(__VA_ARGS__)
#endif // SERIALIZABLE

namespace siraf
{

namespace dynamic
{

class Polymorphic : public Cloneable
{
public:
    using Archive = core::ArchiveBase;

private:
    virtual void __save(Archive& archive) = 0;
    virtual void __load(Archive& archive) = 0;
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_POLYMORPHIC_HPP

