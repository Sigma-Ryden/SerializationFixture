#ifndef SIRAF_DYNAMIC_REGISTRY_BASE_HPP
#define SIRAF_DYNAMIC_REGISTRY_BASE_HPP

#include <Siraf/Core/Access.hpp>
#include <Siraf/Core/PolymorphicTrait.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace dynamic
{

class RegistryBase
{
public:
    using key_type = core::PolymorphicTraitBase::key_type;

public:
    template <class T>
    static key_type key(T& object) noexcept
    {
        return core::Access::trait(object);
    }

    template <class T>
    static key_type key() noexcept
    {
        return core::Access::trait<T>();
    }

public:
    template <class Archive, class Pointer,
              SIREQUIRE(meta::is_pointer_to_polymorphic<Pointer>())>
    static key_type save_key(Archive& archive, Pointer& pointer)
    {
        if (pointer == nullptr)
            throw "The write pointer was not allocated.";

        auto id = key(*pointer);
        archive & id;

        return id;
    }

    template <class Archive, class Pointer,
              SIREQUIRE(meta::is_pointer_to_polymorphic<Pointer>())>
    static key_type load_key(Archive& archive, Pointer& pointer)
    {
        if (pointer != nullptr)
            throw "The read pointer must be initialized to nullptr.";

        key_type id;
        archive & id;

        return id;
    }
};

} // namespace dynamic

} // namespace siraf

#endif // SIRAF_DYNAMIC_REGISTRY_BASE_HPP
