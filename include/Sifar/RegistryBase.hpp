#ifndef SIFAR_REGISTRY_BASE_HPP
#define SIFAR_REGISTRY_BASE_HPP

#include <Sifar/Access.hpp>
#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace detail
{

class RegistryBase
{
public:
    using size_type = std::size_t;

protected:
    static constexpr auto key_max_bound = let::u64(-1);

public:
    template <class T>
    static auto key(T& object) noexcept -> decltype(Access::dynamic_key(object))
    {
        return Access::dynamic_key(object);
    }

    template <class T>
    static constexpr auto key() noexcept -> decltype(Access::static_key<T>())
    {
        return Access::static_key<T>();
    }

    template <class Archive, class Pointer,
              meta::require<meta::is_pointer_to_polymorphic<Pointer>()> = 0>
    static size_type save_key(Archive& archive, Pointer& pointer)
    {
        if (pointer == nullptr)
            throw "the write pointer was not allocated.";

        auto id = Access::dynamic_key(*pointer);
        archive & id;

        return id;
    }

    template <class Archive, class Pointer,
              meta::require<meta::is_pointer_to_polymorphic<Pointer>()> = 0>
    static size_type load_key(Archive& archive, Pointer& /*pointer*/)
    {
        using T = meta::deref<Pointer>;
        using key_type = decltype(Access::template static_key<T>());

        key_type id;
        archive & id;

        return id;
    }

protected:
    template <class Derived, class Archive,
              meta::require<std::is_same<Derived*, void*>::value> = 0>
    static void try_save(Archive& /*archive*/, void* /*pointer*/)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class Archive,
              meta::require<not std::is_same<Derived*, void*>::value> = 0>
    static void try_save(Archive& archive, void* pointer)
    {
        auto derived = Access::template cast<Derived*>(pointer);
        archive & (*derived); // will never nullptr
    }

    template <class Derived, class Archive,
              meta::require<std::is_same<Derived*, void*>::value> = 0>
    static void try_load(Archive& /*archive*/, void*& /*pointer*/)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class Archive,
              meta::require<not std::is_same<Derived*, void*>::value> = 0>
    static void try_load(Archive& archive, void*& pointer)
    {
        if (pointer != nullptr)
            throw "the read pointer must be initialized to nullptr.";

        pointer = new Derived;

        auto derived = Access::template cast<Derived*>(pointer);
        archive & (*derived); // will never nullptr
    }

    template <typename Class, typename Pointer,
              meta::require<not std::is_convertible<Class*, Pointer>::value> = 0>
    static void try_assign(Pointer& pointer, void* address) noexcept { /*pass*/ }

    template <typename Class, typename Pointer,
              meta::require<std::is_convertible<Class*, Pointer>::value> = 0>
    static void try_assign(Pointer& pointer, void* address)
    {
        pointer = Access::template cast<Class*>(address);
    }
};

} // namespace detail

} // namespace sifar

#endif // SIFAR_REGISTRY_BASE_HPP
