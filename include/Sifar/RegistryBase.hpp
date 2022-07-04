#ifndef SIFAR_REGISTRY_BASE_HPP
#define SIFAR_REGISTRY_BASE_HPP

#include <Sifar/Access.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace detail
{

class RegistryBase
{
public:
    using size_type = std::size_t;

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

protected:
    template <class Archive, class P,
              meta::require<meta::is_polymorphic_pointer<P>()> = 0>
    static size_type save_id(Archive& archive, P& pointer)
    {
        if (pointer == nullptr)
            throw "the write pointer was not allocated.";

        auto id = Access::dynamic_key(*pointer);
        archive & id;

        return id;
    }

    template <class Archive, class P,
              meta::require<meta::is_polymorphic_pointer<P>()> = 0>
    static size_type load_id(Archive& archive, P& pointer)
    {
        using T = meta::deref<P>;
        using key_type = decltype(Access::static_key<T>());

        key_type id;
        archive & id;

        return id;
    }

    template <class Derived, class Archive, class Base,
              meta::require<not meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& /*archive*/, Base& /*pointer*/)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class Archive, class Base,
              meta::require<meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& archive, Base& pointer)
    {
        auto derived = Access::template runtime_cast<Derived*>(pointer);
        archive & (*derived); // will never nullptr
    }

    template <class Derived, class Archive, class Base,
              typename B = meta::deref<Base>,
              meta::require<meta::is_abstract<Derived>() or
                            not meta::is_base_of<B, Derived>()> = 0>
    static void load_if_derived_of(Archive& /*archive*/, Base& /*pointer*/)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class Archive, class Base,
              typename B = meta::deref<Base>,
              meta::require<not meta::is_abstract<Derived>() and
                            meta::is_base_of<B, Derived>()> = 0>
    static void load_if_derived_of(Archive& archive, Base& pointer)
    {
        if (pointer != nullptr)
            throw "the read pointer must be initialized to nullptr.";

        auto hold = new Derived;
        pointer = Access::template runtime_cast<B*>(hold);

        auto derived = Access::template runtime_cast<Derived*>(pointer);
        archive & (*derived); // will never nullptr
    }
};

} // namespace detail

} // namespace sifar

#endif // SIFAR_REGISTRY_BASE_HPP
