#ifndef SERIALIZATION_REGISTRY_HPP
#define SERIALIZATION_REGISTRY_HPP

#include <Serialization/Access.hpp>
#include <Serialization/Detail/Meta.hpp>

#define SERIALIZATION_CLASS_INFO(value)                                                                 \
    static constexpr std::size_t static_key() noexcept { return (value); }                              \
    virtual std::size_t dynamic_key() const noexcept { return (value); }

#define SERIALIZATION_CLASS_TPL_INFO(value, ...)                                                        \
    template <> constexpr std::size_t __VA_ARGS__::static_key() noexcept { return (value); }            \
    template <> std::size_t __VA_ARGS__::dynamic_key() const noexcept { return (value); }

#define SERIALIZATION_CLASS_HASH_INFO(...)                                                              \
    SERIALIZATION_CLASS_INFO(::serialization::static_hash(#__VA_ARGS__))

#define SERIALIZATION_CLASS_TPL_HASH_INFO(...)                                                          \
    SERIALIZATION_CLASS_TPL_INFO(::serialization::static_hash(#__VA_ARGS__), __VA_ARGS__)

namespace serialization
{

template <class... Tn>
class Registry
{
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

    template <class Archive, class P,
              typename T = meta::deref<P>,
              typename key_type = decltype(Access::static_key<T>())>
    static void save(Archive& archive, P& pointer, key_type id)
    {
        save_impl<T, Tn..., T>(archive, pointer, id);
    }

    template <class Archive, class P,
              typename T = meta::deref<P>,
              typename key_type = decltype(Access::static_key<T>())>
    static void load(Archive& archive, P& pointer, key_type id)
    {
        load_impl<T, Tn..., T>(archive, pointer, id);
    }

    template <class Archive, class P,
              meta::require<meta::is_polymorphic_pointer<P>()> = 0>
    static void save(Archive& archive, P& pointer)
    {
        if (pointer == nullptr)
            throw "the write pointer was not allocated.";

        auto id = Access::dynamic_key(*pointer);
        archive & id;

        save(archive, pointer, id);
    }

    template <class Archive, class P,
              meta::require<meta::is_polymorphic_pointer<P>()> = 0>
    static void load(Archive& archive, P& pointer)
    {
        using T = meta::deref<P>;
        using key_type = decltype(Access::static_key<T>());

        key_type id;
        archive & id;

        load(archive, pointer, id);
    }

private:
    template <class Derived, class Archive, class Base,
              meta::require<meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& archive, Base& pointer)
    {
        auto derived = Access::template runtime_cast<Derived*>(pointer);
        archive & (*derived); // will never nullptr
    }

    template <class Derived, class Archive, class Base,
              meta::require<not meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& /*archive*/, Base& /*pointer*/) noexcept {}

    template <class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n, class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        if (id == key<Derived>())
            return save_if_derived_of<Derived>(archive, pointer);

        return save_impl<Derived_n...>(archive, pointer, id);
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

    template <class Derived, class Archive, class Base,
              typename B = meta::deref<Base>,
              typename key_type = decltype(Access::static_key<B>()),
              meta::require<meta::is_abstract<Derived>() or
                            not meta::is_base_of<B, Derived>()> = 0>
    static void load_if_derived_of(Archive& /*archive*/, Base& /*pointer*/) noexcept {}

    template <class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n, class Archive, class Base,
              typename key_type = decltype(Access::static_key<meta::deref<Base>>())>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        if (id == key<Derived>())
            return load_if_derived_of<Derived>(archive, pointer);

        return load_impl<Derived_n...>(archive, pointer, id);
    }
};

} // namespace serialization

#endif // SERIALIZATION_REGISTRY_HPP
