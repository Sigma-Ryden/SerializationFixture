#ifndef SIRAF_DYNAMIC_CLONEABLE_HPP
#define SIRAF_DYNAMIC_CLONEABLE_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/Hash.hpp>
#include <Siraf/Core/Memory.hpp>

#define CLONEABLE_BODY(...)                                                                             \
    private:                                                                                            \
    using clone_type = siraf::dynamic::Cloneable::clone_type;                                           \
    using key_type = siraf::dynamic::Cloneable::key_type;                                               \
    private:                                                                                            \
    siraf::dynamic::FactoryRegistry<__VA_ARGS__> __registry;                                            \
    static constexpr key_type __static_trait() noexcept                                                 \
    { return SIRAF_STATIC_HASH(#__VA_ARGS__); }                                                         \
    key_type __trait() const noexcept                                                                   \
    { return ::Serialization::trait<__VA_ARGS__>(); }                                                   \
    std::shared_ptr<clone_type> __shared() const                                                        \
    { return siraf::Memory::allocate_shared<clone_type, __VA_ARGS__>(); }                               \
    std::shared_ptr<clone_type> __cast(std::shared_ptr<void> address) const                             \
    { return siraf::Memory::static_pointer_cast<clone_type, __VA_ARGS__>(address); }                    \
    clone_type* __raw() const                                                                           \
    { return siraf::Memory::allocate_raw<clone_type, __VA_ARGS__>(); }                                  \
    clone_type* __cast(void* address) const                                                             \
    { return siraf::Memory::static_pointer_cast<clone_type, __VA_ARGS__>(address); }                    \
    public:

#ifndef CLONEABLE
    #define CLONEABLE(...)                                                                              \
        public siraf::dynamic::Cloneable
#endif // CLONEABLE

namespace siraf
{

namespace dynamic
{

class Cloneable
{
    friend class FactoryTable;

public:
    using key_type   = core::PolymorphicTraitBase::key_type;
    using clone_type = Cloneable;

public:
    virtual ~Cloneable() = default;

private:
    virtual key_type __trait() const noexcept = 0;

    virtual std::shared_ptr<clone_type> __shared() const = 0;
    virtual std::shared_ptr<clone_type> __cast(std::shared_ptr<void> address) const = 0;

    virtual clone_type* __raw() const = 0;
    virtual clone_type* __cast(void* address) const = 0;
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_CLONEABLE_HPP
