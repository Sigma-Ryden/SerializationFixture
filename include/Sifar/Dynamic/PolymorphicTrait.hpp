#ifndef SIFAR_POLYMORPHIC_TRAIT_HPP
#define SIFAR_POLYMORPHIC_TRAIT_HPP

#include <Sifar/Hash.hpp>

// EXPORT_POLYMORPHIC_KEY - only for translation unit (.cpp) or templates (.hpp)
#define EXPORT_POLYMORPHIC_KEY(unique_name, ...)                                                        \
    namespace sifar { namespace dynamic {                                                               \
        template <> struct PolymorphicTraitKey<__VA_ARGS__>                                             \
        { static constexpr auto key = SIFAR_STATIC_HASH(unique_name); };                                \
    }}

#define EXPORT_POLYMORPHIC(...)                                                                         \
    EXPORT_POLYMORPHIC_KEY(#__VA_ARGS__, __VA_ARGS__)

namespace sifar
{

namespace dynamic
{

struct PolymorphicTraitCore
{
    using key_type = SIFAR_STATIC_HASH_KEY_TYPE;
    
    static constexpr auto base_key = key_type(-1);
};

template <class T>
struct PolymorphicTraitKey
{
    static constexpr auto key = PolymorphicTraitCore::base_key;
};

} // namespace dynamic

} // namespace sifar

#endif // SIFAR_POLYMORPHIC_TRAIT_HPP
