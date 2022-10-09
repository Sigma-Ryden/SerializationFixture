#ifndef SIFAR_REGISTRY_BASE_HPP
#define SIFAR_REGISTRY_BASE_HPP

#include <Sifar/Access.hpp>
#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace dynamic
{

class RegistryBase
{
public:
    using key_type = let::u64;

public:
    template <class T>
    static key_type key(T& object) noexcept
    {
        return Access::dynamic_key(object);
    }

    template <class T>
    static constexpr key_type key() noexcept
    {
        return Access::static_key<T>();
    }

    template <class Archive, class Pointer,
              SIFAR_REQUIRE(meta::is_pointer_to_polymorphic<Pointer>())>
    static key_type save_key(Archive& archive, Pointer& pointer)
    {
        if (pointer == nullptr)
            throw "the write pointer was not allocated.";

        auto id = Access::dynamic_key(*pointer);
        archive & id;

        return id;
    }

    template <class Archive, class Pointer,
              SIFAR_REQUIRE(meta::is_pointer_to_polymorphic<Pointer>())>
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

} // namespace sifar

#endif // SIFAR_REGISTRY_BASE_HPP
