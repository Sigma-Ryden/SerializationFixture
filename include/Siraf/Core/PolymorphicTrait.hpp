#ifndef SIRAF_CORE_POLYMORPHIC_TRAIT_HPP
#define SIRAF_CORE_POLYMORPHIC_TRAIT_HPP

#include <Siraf/Core/Hash.hpp>

#define EXPORT_POLYMORPHIC_KEY(unique_name, ...)                                                        \
    namespace siraf { namespace core {                                                                  \
        template <> struct PolymorphicTraitKey<__VA_ARGS__> {                                           \
            static constexpr auto key = SIRAF_STATIC_HASH(unique_name);                                 \
        };                                                                                              \
    }}

#define EXPORT_POLYMORPHIC(...)                                                                         \
    EXPORT_POLYMORPHIC_KEY(#__VA_ARGS__, __VA_ARGS__)

namespace siraf
{

namespace core
{

struct PolymorphicTraitBase
{
    using key_type = SIRAF_STATIC_HASH_KEY_TYPE;
    
    static constexpr auto base_key = key_type(-1);
};

template <class T>
struct PolymorphicTraitKey
{
    static constexpr auto key = PolymorphicTraitBase::base_key;
};

} // namespace core

} // namespace siraf

#endif // SIRAF_CORE_POLYMORPHIC_TRAIT_HPP
