#ifndef SERIALIZATION_REGISTRY_HPP
#define SERIALIZATION_REGISTRY_HPP

#include "Detail/Meta.hpp"

#define SERIALIZATION_IMPLEMENT_CLASS_INFO(...)                                                         \
    static constexpr std::size_t static_index() noexcept {                                              \
        return ::serialization::static_hash(#__VA_ARGS__);                                              \
    }                                                                                                   \
    virtual std::size_t index() const noexcept {                                                        \
        return static_index();                                                                          \
    }

#define SERIALIZATION_IMPLEMENT_CLASS_TPL_INFO(...)                                                     \
    template<>                                                                                          \
    constexpr std::size_t __VA_ARGS__::static_index() noexcept {                                        \
        return ::serialization::static_hash(#__VA_ARGS__);                                              \
    }                                                                                                   \
    template<>                                                                                          \
    std::size_t __VA_ARGS__::index() const noexcept {                                                   \
        return static_index();                                                                          \
    }

namespace serialization
{

// will be repair
template <typename To, typename From>
To& base(From& derived)
{
    return static_cast<To&>(derived);
}

template <class... Tn>
class Registry
{
public:
    using index_type = std::size_t;

public:
    template <class T>
    static auto index(T* object) noexcept -> decltype(object->index())
    {
        return object->index();
    }

    template <class T>
    static auto index() noexcept -> decltype(T::static_index())
    {
        return T::static_index();
    }

    template <class Archive, class P, typename T = meta::deref<P>>
    static void save(Archive& archive, P& pointer, index_type id)
    {
        return save_impl<T, Tn..., T>(archive, pointer, id);
    }

    template <class Archive, class P, typename T = meta::deref<P>>
    static void load(Archive& archive, P& pointer, index_type id)
    {
        return load_impl<T, Tn..., T>(archive, pointer, id);
    }

private:
    template <class Derived, class Archive, class Base,
              meta::require<meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& archive, Base& pointer)
    {
        archive & static_cast<Derived&>(*pointer);
    }

    template <class Derived, class Archive, class Base,
              meta::require<not meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void save_if_derived_of(Archive& /*archive*/, Base& /*pointer*/) noexcept {}

    template <class Archive, class Base>
    static void save_impl(Archive& archive, Base& pointer, index_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n, class Archive, class Base>
    static void save_impl(Archive& archive, Base& pointer, index_type id)
    {
        if (id == index<Derived>())
            return save_if_derived_of<Derived>(archive, pointer);

        return save_impl<Derived_n...>(archive, pointer, id);
    }

    template <class Derived, class Archive, class Base,
              meta::require<not meta::is_abstract<Derived>() and
                            meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void load_if_derived_of(Archive& archive, Base& pointer)
    {
        if (pointer) delete pointer;

        pointer = new Derived;

        archive & (static_cast<Derived&>(*pointer));
    }

    template <class Derived, class Archive, class Base,
              meta::require<meta::is_abstract<Derived>() or
                            not meta::is_base_of<meta::deref<Base>, Derived>()> = 0>
    static void load_if_derived_of(Archive& /*archive*/, Base& /*pointer*/) noexcept {}

    template <class Archive, class Base>
    static void load_impl(Archive& archive, Base& pointer, index_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n, class Archive, class Base>
    static void load_impl(Archive& archive, Base& pointer, index_type id)
    {
        if (id == index<Derived>())
            return load_if_derived_of<Derived>(archive, pointer);

        return load_impl<Derived_n...>(archive, pointer, id);
    }
};

} // namespace serialization

#endif // SERIALIZATION_REGISTRY_HPP
