#ifndef SIRAF_DYNAMIC_POLYMORPHIC_HPP
#define SIRAF_DYNAMIC_POLYMORPHIC_HPP

#include <Siraf/Core/ArchiveBase.hpp>

#include <Siraf/Dynamic/FactoryTable.hpp> // Cloneable
#include <Siraf/Dynamic/PolymorphicArchive.hpp> // operator <<, operator >>

// override will implicit add, when class inherit Polymorphic
#define POLYMORPHIC_BODY(...)                                                                           \
    private:                                                                                            \
    void __save(siraf::core::ArchiveBase& archive) { archive << *this; }                                \
    void __load(siraf::core::ArchiveBase& archive) { archive >> *this; }                                \
    public:

#ifndef POLYMORPHIC
    #define POLYMORPHIC(...)                                                                            \
        public siraf::dynamic::Polymorphic
#endif // POLYMORPHIC

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

