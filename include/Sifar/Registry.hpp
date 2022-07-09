#ifndef SIFAR_REGISTRY_HPP
#define SIFAR_REGISTRY_HPP

#include <Sifar/RegistryBase.hpp>

#include <Sifar/Detail/Meta.hpp>

#define SERIALIZATION_POLYMORPHIC_KEY(value)                                                            \
    static constexpr std::size_t static_key() noexcept { return (value); }                              \
    virtual std::size_t dynamic_key() const noexcept { return static_key(); }

#define SERIALIZATION_POLYMORPHIC_HASH(...)                                                             \
    SERIALIZATION_POLYMORPHIC_KEY(::sifar::static_hash(#__VA_ARGS__))

#define SERIALIZATION_POLYMORPHIC()                                                                     \
    static constexpr std::size_t static_key() noexcept;                                                 \
    virtual std::size_t dynamic_key() const noexcept;

#define SERIALIZATION_EXPORT_KEY(...)                                                                   \
    template <> struct sifar::class_export<                                                             \
    ::sifar::meta::check_depth< ::sifar::Access::static_key<__VA_ARGS__>() >::value>                    \
    { using type = __VA_ARGS__; };

#define SERIALIZATION_EXPORT(...)                                                                       \
    COUNTER_INCREMENT()                                                                                 \
    constexpr std::size_t __VA_ARGS__::static_key() noexcept { return COUNTER_VALUE(); }                \
    std::size_t __VA_ARGS__::dynamic_key() const noexcept { return static_key(); }                      \
    SERIALIZATION_EXPORT_KEY(__VA_ARGS__)

#define SERIALIZATION_POLYMORPHIC_TPL_KEY(value, ...)                                                   \
    template <> constexpr std::size_t __VA_ARGS__::static_key() noexcept { return (value); }

#define SERIALIZATION_POLYMORPHIC_TPL_HASH(...)                                                         \
    SERIALIZATION_POLYMORPHIC_TPL_KEY(::sifar::static_hash(#__VA_ARGS__), __VA_ARGS__)

#define SERIALIZATION_EXPORT_TPL_KEY(value, ...)                                                        \
    SERIALIZATION_POLYMORPHIC_TPL_KEY(value, __VA_ARGS__)                                               \
    SERIALIZATION_EXPORT_KEY(__VA_ARGS__)

#define SERIALIZATION_EXPORT_TPL(...)                                                                   \
    COUNTER_INCREMENT()                                                                                 \
    template <> constexpr std::size_t __VA_ARGS__::static_key() noexcept { return COUNTER_VALUE(); }    \
    template <> std::size_t __VA_ARGS__::dynamic_key() const noexcept { return static_key(); }          \
    SERIALIZATION_EXPORT_KEY(__VA_ARGS__)

namespace sifar
{

namespace meta
{

template <std::size_t Key>
struct check_depth
{
private:
    static constexpr bool require = Key < sifar::meta::max_template_depth();
    static_assert(require, "'Key' should be less than 'sifar::detail::max_template_depth()'.");

public:
    static constexpr std::size_t value = Key;
};

} // namespace meta

template <std::size_t I>
struct class_export
{
    using type = void;
};

class ExternRegistry : public detail::RegistryBase
{
public:
    template <class Archive, typename Pointer, typename key_type>
    static void save(Archive& archive, Pointer& pointer, key_type id)
    {
        save_impl<0, meta::max_template_depth()>(archive, pointer, id);
    }

    template <class Archive, typename Pointer, typename key_type>
    static void load(Archive& archive, Pointer& pointer, key_type id)
    {
        load_impl<0, meta::max_template_depth()>(archive, pointer, id);
    }

    template <typename Pointer, typename key_type>
    static void assign(Pointer& pointer, void* address, key_type id)
    {
        assign_impl<0, meta::max_template_depth()>(pointer, address, id);
    }

private:
    template <size_type StaticKey, size_type StaticKeyLimit,
              class Archive, class Base, typename key_type,
              meta::require<(StaticKey == StaticKeyLimit)> = 0>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <size_type StaticKey, size_type StaticKeyLimit,
              class Archive, class Base, typename key_type,
              meta::require<(StaticKey < StaticKeyLimit)> = 0>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        using Derived = typename class_export<StaticKey>::type;

        if (id == StaticKey)
            return save_if_derived_of<Derived>(archive, pointer);

        save_impl<StaticKey + 1, StaticKeyLimit>(archive, pointer, id);
    }

    template <size_type StaticKey, size_type StaticKeyLimit,
              class Archive, class Base, typename key_type,
              meta::require<(StaticKey == StaticKeyLimit)> = 0>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <size_type StaticKey, size_type StaticKeyLimit,
              class Archive, class Base, typename key_type,
              meta::require<(StaticKey < StaticKeyLimit)> = 0>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        using Derived = typename class_export<StaticKey>::type;

        if (id == StaticKey)
            return load_if_derived_of<Derived>(archive, pointer);

        load_impl<StaticKey + 1, StaticKeyLimit>(archive, pointer, id);
    }

    template <size_type StaticKey, size_type StaticKeyLimit,
              typename Pointer, typename key_type,
              meta::require<(StaticKey == StaticKeyLimit)> = 0>
    static void assign_impl(Pointer& pointer, void* address, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <size_type StaticKey, size_type StaticKeyLimit,
              typename Pointer, typename key_type,
              meta::require<(StaticKey < StaticKeyLimit)> = 0>
    static void assign_impl(Pointer& pointer, void* address, key_type id)
    {
        using Class = typename class_export<StaticKey>::type;

        if (id == StaticKey)
            return try_assign<Class>(pointer, address);

        assign_impl<StaticKey + 1, StaticKeyLimit>(pointer, address, id);
    }
};

template <class... Tn>
class InnerRegistry : public detail::RegistryBase
{
public:
    template <class Archive, typename Pointer, typename key_type>
    static void save(Archive& archive, Pointer& pointer, key_type id)
    {
        using T = meta::deref<Pointer>;
        save_impl<T, Tn..., T>(archive, pointer, id);
    }

    template <class Archive, typename Pointer, typename key_type>
    static void load(Archive& archive, Pointer& pointer, key_type id)
    {
        using T = meta::deref<Pointer>;
        load_impl<T, Tn..., T>(archive, pointer, id);
    }

    template <typename Pointer, typename key_type>
    static void assign(Pointer& pointer, void* address, key_type id)
    {
        using T = meta::deref<Pointer>;
        assign_impl<T, Tn..., T>(pointer, address, id);
    }

private:
    template <class Archive, class Base, typename key_type>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n,
              class Archive, class Base, typename key_type>
    static void save_impl(Archive& archive, Base& pointer, key_type id)
    {
        if (id == key<Derived>())
            return save_if_derived_of<Derived>(archive, pointer);

        save_impl<Derived_n...>(archive, pointer, id);
    }

    template <class Archive, class Base, typename key_type>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n,
              class Archive, class Base, typename key_type>
    static void load_impl(Archive& archive, Base& pointer, key_type id)
    {
        if (id == key<Derived>())
            return load_if_derived_of<Derived>(archive, pointer);

        load_impl<Derived_n...>(archive, pointer, id);
    }

    template <typename Pointer, typename key_type>
    static void assign_impl(Pointer& pointer, void* address, key_type id)
    {
        throw "serializable type was not registered.";
    }

    template <class Class, class... Class_n,
              typename Pointer, typename key_type>
    static void assign_impl(Pointer& pointer, void* address, key_type id)
    {
        if (id == key<Class>())
            return try_assign<Class>(pointer, address);

        assign_impl<Class_n...>(pointer, address, id);
    }
};

} // namespace sifar

#endif // SIFAR_REGISTRY_HPP
