#ifndef SIFAR_REGISTRY_HPP
#define SIFAR_REGISTRY_HPP

#include <Sifar/RegistryBase.hpp>
#include <Sifar/Utility.hpp>

#include <Sifar/Detail/Invoker.hpp>
#include <Sifar/Detail/Meta.hpp>

#define SERIALIZATION_POLYMORPHIC_KEY(value)                                                            \
    static constexpr std::size_t static_key() noexcept { return (value); }                              \
    virtual std::size_t dynamic_key() const noexcept { return static_key(); }

#define SERIALIZATION_POLYMORPHIC_HASH(...)                                                             \
    SERIALIZATION_POLYMORPHIC_KEY(::sifar::static_hash(#__VA_ARGS__))

#define SERIALIZATION_EXPORT_KEY(...)                                                                   \
    template <> struct sifar::class_export<                                                             \
    ::sifar::meta::check_depth< ::sifar::Access::static_key<__VA_ARGS__>() >::value>                    \
    { using type = __VA_ARGS__; };

#define SERIALIZATION_POLYMORPHIC_TPL_KEY(value, ...)                                                   \
    template <> constexpr std::size_t __VA_ARGS__::static_key() noexcept { return (value); }

#define SERIALIZATION_POLYMORPHIC_TPL_HASH(...)                                                         \
    SERIALIZATION_POLYMORPHIC_TPL_KEY(::sifar::static_hash(#__VA_ARGS__), __VA_ARGS__)

#define SERIALIZATION_EXPORT_TPL_KEY(value, ...)                                                        \
    SERIALIZATION_POLYMORPHIC_TPL_KEY(value, __VA_ARGS__)                                               \
    SERIALIZATION_EXPORT_KEY(__VA_ARGS__)

#ifdef SIFAR_EXPERIMENTAL
#define SERIALIZATION_POLYMORPHIC()                                                                     \
    static constexpr std::size_t static_key() noexcept;                                                 \
    virtual std::size_t dynamic_key() const noexcept;

#define SERIALIZATION_EXPORT(...)                                                                       \
    COUNTER_INCREMENT()                                                                                 \
    constexpr std::size_t __VA_ARGS__::static_key() noexcept { return COUNTER_VALUE(); }                \
    std::size_t __VA_ARGS__::dynamic_key() const noexcept { return static_key(); }                      \
    SERIALIZATION_EXPORT_KEY(__VA_ARGS__)

#define SERIALIZATION_EXPORT_TPL(...)                                                                   \
    COUNTER_INCREMENT()                                                                                 \
    template <> constexpr std::size_t __VA_ARGS__::static_key() noexcept { return COUNTER_VALUE(); }    \
    template <> std::size_t __VA_ARGS__::dynamic_key() const noexcept { return static_key(); }          \
    SERIALIZATION_EXPORT_KEY(__VA_ARGS__)
#endif // SIFAR_EXPERIMENTAL

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
    template <class Archive, typename Pointer,
              class T = meta::deref<Pointer>,
              meta::require<Access::is_registered_class<T>()> = 0>
    static void save(
        Archive& archive, Pointer& pointer,
        let::u64 key, let::u64 bound = key_max_bound)
    {
        void* pure_pointer = dynamic_cast<void*>(pointer);

        save_impl(archive, pure_pointer, key % bound);
    }

    template <class Archive, typename Pointer,
              class T = meta::deref<Pointer>,
              meta::require<Access::is_registered_class<T>()> = 0>
    static void load(
        Archive& archive, Pointer& pointer,
        let::u64 key, let::u64 bound = key_max_bound)
    {
        void* pure_pointer = nullptr;

        load_impl(archive, pure_pointer, key % bound);

        pointer = static_cast<Pointer>(pure_pointer);
    }

    template <typename Pointer,
              class T = meta::deref<Pointer>,
              meta::require<Access::is_registered_class<T>()> = 0>
    static void assign(
        Pointer& pointer, void* address,
        let::u64 key, let::u64 bound = key_max_bound)
    {
        assign_impl(pointer, address, key % bound);
    }

private:
    template <SIFAR_INVOKE_END(StaticKey), class Archive>
    static void save_impl(Archive& archive, void* pointer, let::u64 id)
    {
        throw "serializable type was not registered.";
    }

    template <SIFAR_INVOKE_BEGIN(StaticKey), class Archive>
    static void save_impl(Archive& archive, void* pointer, let::u64 id)
    {
        SIFAR_INVOKE_BODY(StaticKey, id, save_impl<INVOKE_NEXT>(archive, pointer, id))

        using Derived = typename class_export<StaticKey>::type;

        try_save<Derived>(archive, pointer);
    }

    template <SIFAR_INVOKE_END(StaticKey), class Archive>
    static void load_impl(Archive& archive, void*& pointer, let::u64 id)
    {
        throw "serializable type was not registered.";
    }

    template <SIFAR_INVOKE_BEGIN(StaticKey), class Archive>
    static void load_impl(Archive& archive, void*& pointer, let::u64 id)
    {
        SIFAR_INVOKE_BODY(StaticKey, id, load_impl<INVOKE_NEXT>(archive, pointer, id))

        using Derived = typename class_export<StaticKey>::type;
        try_load<Derived>(archive, pointer);
    }

    template <SIFAR_INVOKE_END(StaticKey), typename Pointer>
    static void assign_impl(Pointer& pointer, void* address, let::u64 id)
    {
        throw "serializable type was not registered.";
    }

    template <SIFAR_INVOKE_BEGIN(StaticKey), typename Pointer>
    static void assign_impl(Pointer& pointer, void* address, let::u64 id)
    {
        SIFAR_INVOKE_BODY(StaticKey, id, assign_impl<INVOKE_NEXT>(pointer, address, id))

        using Class = typename class_export<StaticKey>::type;
        try_assign<Class>(pointer, address);
    }
};

template <class... Tn>
class InnerRegistry : public detail::RegistryBase
{
public:
    template <class Archive, typename Pointer,
              class T = meta::deref<Pointer>,
              meta::require<Access::is_registered_class<T>()> = 0>
    static void save(
        Archive& archive, Pointer& pointer,
        let::u64 key, let::u64 bound = key_max_bound)
    {
        void* pure_pointer = dynamic_cast<void*>(pointer);

        save_impl<T, Tn..., T>(archive, pure_pointer, key % bound);
    }

    template <class Archive, typename Pointer,
              class T = meta::deref<Pointer>,
              meta::require<Access::is_registered_class<T>()> = 0>
    static void load(
        Archive& archive, Pointer& pointer,
        let::u64 key, let::u64 bound = key_max_bound)
    {
        void* pure_pointer = nullptr;

        load_impl<T, Tn..., T>(archive, pure_pointer, key % bound);

        pointer = static_cast<Pointer>(pure_pointer);
    }

    template <typename Pointer,
              class T = meta::deref<Pointer>,
              meta::require<Access::is_registered_class<T>()> = 0>
    static void assign(
        Pointer& pointer, void* address,
        let::u64 key, let::u64 bound = key_max_bound)
    {
        assign_impl<T, Tn..., T>(pointer, address, key % bound);
    }

private:
    template <class Archive>
    static void save_impl(Archive& archive, void* pointer, let::u64 id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n,
              class Archive>
    static void save_impl(Archive& archive, void* pointer, let::u64 id)
    {
        if (id == key<Derived>())
            return try_save<Derived>(archive, pointer);

        save_impl<Derived_n...>(archive, pointer, id);
    }

    template <class Archive>
    static void load_impl(Archive& archive, void*& pointer, let::u64 id)
    {
        throw "serializable type was not registered.";
    }

    template <class Derived, class... Derived_n,
              class Archive>
    static void load_impl(Archive& archive, void*& pointer, let::u64 id)
    {
        if (id == key<Derived>())
            return try_load<Derived>(archive, pointer);

        load_impl<Derived_n...>(archive, pointer, id);
    }

    template <typename Pointer>
    static void assign_impl(Pointer& pointer, void* address, let::u64 id)
    {
        throw "serializable type was not registered.";
    }

    template <class Class, class... Class_n,
              typename Pointer>
    static void assign_impl(Pointer& pointer, void* address, let::u64 id)
    {
        if (id == key<Class>())
            return try_assign<Class>(pointer, address);

        assign_impl<Class_n...>(pointer, address, id);
    }
};

} // namespace sifar

#endif // SIFAR_REGISTRY_HPP
