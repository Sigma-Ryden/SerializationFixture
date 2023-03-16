#ifndef SIRAF_DYNAMIC_POLYMORPHIC_HPP
#define SIRAF_DYNAMIC_POLYMORPHIC_HPP

#include <Siraf/Core/ArchiveBase.hpp>

#include <Siraf/Core/PolymorphicArchive.hpp> // operator <<, operator >>
#include <Siraf/Dynamic/FactoryTable.hpp> // Cloneable

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
    virtual ~Polymorphic() = default;

private:
    virtual void __save(core::ArchiveBase& archive) = 0;
    virtual void __load(core::ArchiveBase& archive) = 0;
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_POLYMORPHIC_HPP

