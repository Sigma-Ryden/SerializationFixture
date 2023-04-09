#ifndef SF_DYNAMIC_INSTANTIABLE_HPP
#define SF_DYNAMIC_INSTANTIABLE_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/InstantiableTrait.hpp>
#include <SF/Core/Hash.hpp>
#include <SF/Core/Memory.hpp>

// By default library will use Instantiable type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::sf::Instantiable
#endif // INSTANTIABLE_TYPE

#define INSTANTIABLE_BODY(...)                                                                          \
    private:                                                                                            \
    using key_type = sf::core::InstantiableTraitBase::key_type;                                         \
    sf::dynamic::InstantiableFixture<__VA_ARGS__> __fixture;                                            \
    static constexpr key_type __static_trait() noexcept { return SF_STATIC_HASH(#__VA_ARGS__); }        \
    virtual key_type __trait() const noexcept { return ::Serialization::trait<__VA_ARGS__>(); }         \
    public:

namespace sf
{

// Should be in sf namespace
struct Instantiable { virtual ~Instantiable() = default; };

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_HPP
