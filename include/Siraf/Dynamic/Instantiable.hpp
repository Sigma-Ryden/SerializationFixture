#ifndef SIRAF_DYNAMIC_INSTANTIABLE_HPP
#define SIRAF_DYNAMIC_INSTANTIABLE_HPP

#include <Siraf/Core/Serialization.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>
#include <Siraf/Core/Hash.hpp>
#include <Siraf/Core/Memory.hpp>

// By default library will use Instantiable type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::siraf::Instantiable
#endif // INSTANTIABLE_TYPE

#define INSTANTIABLE_BODY(...)                                                                          \
    private:                                                                                            \
    using key_type = siraf::core::PolymorphicTraitBase::key_type;                                       \
    siraf::dynamic::InstantiableFixture<__VA_ARGS__> __fixture;                                         \
    static constexpr key_type __static_trait() noexcept { return SIRAF_STATIC_HASH(#__VA_ARGS__); }     \
    virtual key_type __trait() const noexcept { return ::Serialization::trait<__VA_ARGS__>(); }         \
    public:

namespace siraf
{

// Should be in siraf namespace
struct Instantiable { virtual ~Instantiable() = default; };

} // namespace siraf

#endif // SIRAF_DYNAMIC_INSTANTIABLE_HPP
