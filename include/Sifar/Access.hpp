// You MUST NOT include other files in this file, than meta and macros.
#ifndef SIFAR_ACCESS_HPP
#define SIFAR_ACCESS_HPP

#include <Sifar/Serializable.hpp>

#include <Sifar/ApplyFunctor.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

#define SERIALIZABLE(...)                                                                               \
    friend class ::SifarSerializable;                                                                   \
    friend class ::sifar::Access;

#define _SIFAR_HAS_PROPERTY_HELPER(name)                                                                \
    template <typename C, typename = void>                                                              \
    struct has_##name : std::false_type {};								                                \
    template <typename C>                                                                               \
    struct has_##name<C, ::sifar::meta::to_void<decltype(&C::name)>>                                    \
        : std::true_type {}

#define _SIFAR_APPLY_FUNCTOR_GENERIC(class_name, function_name)                                         \
    template <typename Derived, typename Base>                                                          \
    class class_name : public ApplyFunctor {                                                            \
    private:                                                                                            \
        Derived& object_;                                                                               \
    public:                                                                                             \
        class_name(Derived& object) : object_(object) {}                                                \
        template <typename Archive, SIREQUIRE(meta::is_archive<Archive>())>                             \
        void operator() (Archive& archive)                                                              \
        { function_name<Base>(archive, object_); }                                                      \
    };

#define _SIFAR_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(class_name, function_name)                        \
    template <typename Base, typename Derived,                                                          \
              SIREQUIRE(meta::is_base_of<Base, Derived>())>                                             \
    apply::class_name<Derived, Base> function_name(Derived& object)                                     \
    { return { object }; }                                                                              \

namespace sifar
{

namespace core
{

class ArchiveBase;

} // namespace core

class Access
{
    using Serializable = SifarSerializable;

private:
    _SIFAR_HAS_PROPERTY_HELPER(save);
    _SIFAR_HAS_PROPERTY_HELPER(load);

    _SIFAR_HAS_PROPERTY_HELPER(static_key);
    _SIFAR_HAS_PROPERTY_HELPER(dynamic_key);

    _SIFAR_HAS_PROPERTY_HELPER(pure);

public:
    template <class T>
    static constexpr bool is_save_class() noexcept
    {
        return not has_pure<Serializable::Save<T>>::value;
    }

    template <class T>
    static constexpr bool is_load_class() noexcept
    {
        return not has_pure<Serializable::Load<T>>::value;
    }

    template <class T>
    static constexpr bool is_save_load_class() noexcept
    {
        return is_save_class<T>() and is_load_class<T>();
    }

    template <class T>
    static constexpr bool is_dynamic_save_class() noexcept
    {
        return has_save<T>::value;
    }

    template <class T>
    static constexpr bool is_dynamic_load_class() noexcept
    {
        return has_load<T>::value;
    }

    template <class T>
    static constexpr bool is_registered_class() noexcept
    {
        return has_static_key<T>::value and has_dynamic_key<T>::value;
    }

private:
    template <class, class> static std::false_type try_cast(...);
    template <class From, class To> static auto try_cast(int) -> decltype
    (
        (void) std::declval<void(&)(To)>()(std::declval<From>()),
        std::true_type{}
    );

public:
    template <class From, class To>
    static constexpr bool is_convertible() noexcept
    {
        return (meta::is_returnable<To>() and decltype(try_cast<From, To>(0))::value)
            or (meta::is_same_all<void, From, To>());
    }

public:
    template <class Archive, class T,
              SIREQUIRE(not meta::is_write_archive<Archive>() or
                        not is_save_class<T>())>
    static void save(Archive& archive, T& object)
    {
        throw "The 'T' type cannot be saved.";
    }

    template <class Archive, class T,
              SIREQUIRE(meta::is_write_archive<Archive>() and
                        is_save_class<T>())>
    static void save(Archive& archive, T& object)
    {
        Serializable::Save<T>::call(archive, object);
    }

    template <class Archive, class T,
              SIREQUIRE(not meta::is_read_archive<Archive>() or
                        not is_load_class<T>())>
    static void load(Archive& archive, T& object)
    {
        throw "The 'T' type cannot be loaded.";
    }

    template <class Archive, class T,
              SIREQUIRE(meta::is_read_archive<Archive>() and
                        is_load_class<T>())>
    static void load(Archive& archive, T& object)
    {
        Serializable::Load<T>::call(archive, object);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(is_dynamic_save_class<T>() and
                        is_save_class<T>())>
    static void dynamic_save(core::ArchiveBase& archive, Pointer& object)
    {
        object->save(archive);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(is_dynamic_load_class<T>() and
                        is_load_class<T>())>
    static void dynamic_load(core::ArchiveBase& archive, Pointer& object)
    {
        object->load(archive);
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static auto dynamic_key(T& object) noexcept -> decltype(object.dynamic_key())
    {
        return object.dynamic_key();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static constexpr auto static_key() noexcept -> decltype(T::static_key())
    {
        return T::static_key();
    }

private:
    template <class To, class From>
    static To cast(From& from) noexcept
    {
        return static_cast<To>(from);
    }

    template <class To, class From>
    static To runtime_cast(From& from) // maybe throw exception
    {
        return dynamic_cast<To>(from);
    }

public: // not necessary, friend search applied via ADL or external declaration
    template <typename Base, class Archive, typename Derived,
              meta::require<meta::is_archive<Archive>() and
                            meta::is_base_of<Base, Derived>()> = 0>
    friend void base(Archive& archive, Derived& object) noexcept
    {
        archive & Access::template cast<Base&>(object);
    }
};

// Declaration friend function template for the Access class
template <typename Base, class Archive, typename Derived,
          meta::require<meta::is_archive<Archive>() and
                        meta::is_base_of<Base, Derived>()>>
void base(Archive& archive, Derived& object) noexcept;

template <typename Base, class Archive, typename Derived,
          SIREQUIRE(meta::is_base_of<Base, Derived>())>
void virtual_base(Archive& archive, Derived& object) noexcept
{
    if (Access::dynamic_key(object) == Access::template static_key<Derived>())
        base<Base>(archive, object);

#ifdef SIFAR_DEBUG
    else throw "the srializable object must serialize the virtual base object.";
#endif
}

template <class Base, class Archive, class Derived,
          SIREQUIRE(meta::can_static_cast<Base, Derived>() and
                    meta::is_base_of<Base, Derived>())>
void native_base(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SIREQUIRE(not meta::can_static_cast<Base, Derived>() and
                    meta::is_base_of<Base, Derived>())>
void native_base(Archive& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

namespace apply
{

_SIFAR_APPLY_FUNCTOR_GENERIC(BaseFunctor, base)
_SIFAR_APPLY_FUNCTOR_GENERIC(VirtualBaseFunctor, virtual_base)
_SIFAR_APPLY_FUNCTOR_GENERIC(NativeBaseFunctor, native_base)

} // namespace apply

_SIFAR_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(BaseFunctor, base)
_SIFAR_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(VirtualBaseFunctor, virtual_base)
_SIFAR_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(NativeBaseFunctor, native_base)

// Variadic native_base function
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) { /*pass*/ }

template <class Base, class... Base_n, class Archive, class Derived,
          SIREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>())>
void hierarchy(Archive& archive, Derived& object)
{
    native_base<Base>(archive, object);
    hierarchy<Base_n...>(archive, object);
}

namespace apply
{

template <typename Derived, typename Base, typename... Base_n>
class HierarchyFunctor : public ApplyFunctor
{
private:
    Derived& object_;

public:
    HierarchyFunctor(Derived& object) : object_(object) {}

    template <typename Archive, SIREQUIRE(meta::is_archive<Archive>())>
    void operator() (Archive& archive)
    {
        hierarchy<Base, Base_n...>(archive, object_);
    }
};

} // namespace apply

template <class Base, class... Base_n, class Derived,
          SIREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>())>
apply::HierarchyFunctor<Derived, Base, Base_n...> hierarchy(Derived& object)
{
    return { object };
}

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(Access::is_save_class<T>() or Access::is_load_class<T>())

// clean up
#undef _SIFAR_HAS_PROPERTY_HELPER
#undef _SIFAR_APPLY_FUNCTOR_GENERIC
#undef _SIFAR_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC

#endif // SIFAR_ACCESS_HPP
