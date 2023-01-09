// You MUST NOT include other files in this file, than meta and macros.
#ifndef SIRAF_ACCESS_HPP
#define SIRAF_ACCESS_HPP

#include <Siraf/Serializable.hpp>

#include <Siraf/ApplyFunctor.hpp>

#include <Siraf/Dynamic/PolymorphicTrait.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

#define SERIALIZATION_ACCESS(...)                                                                       \
    friend class ::Serialization;                                                                       \
    friend class ::siraf::Access;

#define _SIRAF_HAS_PROPERTY_HELPER(extern_name, inner_name)                                             \
    template <typename C, typename = void>                                                              \
    struct has_##extern_name : std::false_type {};                                                      \
    template <typename C>                                                                               \
    struct has_##extern_name<C, ::siraf::meta::to_void<decltype(&C::inner_name)>>                       \
        : std::true_type {};

#define _SIRAF_APPLY_FUNCTOR_GENERIC(class_name, function_name)                                         \
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

#define _SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(class_name, function_name)                        \
    template <typename Base, typename Derived,                                                          \
              SIREQUIRE(meta::is_base_of<Base, Derived>())>                                             \
    apply::class_name<Derived, Base> function_name(Derived& object)                                     \
    { return { object }; }

namespace siraf
{

namespace core
{

class ArchiveBase;

} // namespace core

class Access
{
private:
    using Sr = Serialization;

public:
    template <typename T>
    using SaveMode = meta::scope<Sr::Save<T>, Sr::SaveLoad<T>, Sr>;

    template <typename T>
    using LoadMode = meta::scope<Sr::Load<T>, Sr::SaveLoad<T>, Sr>;

private:
    _SIRAF_HAS_PROPERTY_HELPER(save, __save)
    _SIRAF_HAS_PROPERTY_HELPER(load, __load)

    _SIRAF_HAS_PROPERTY_HELPER(static_trait, __static_trait)
    _SIRAF_HAS_PROPERTY_HELPER(trait, __trait)

    _SIRAF_HAS_PROPERTY_HELPER(pure, pure)

public:
    template <class T>
    static constexpr bool is_save_class() noexcept
    {
        return not has_pure<Sr::Save<T>>::value;
    }

    template <class T>
    static constexpr bool is_load_class() noexcept
    {
        return not has_pure<Sr::Load<T>>::value;
    }

    template <class T>
    static constexpr bool is_saveload_class() noexcept
    {
        return not has_pure<Sr::SaveLoad<T>>::value;
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
        return has_static_trait<T>::value and has_trait<T>::value;
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
    template <typename Archive, typename T>
    static void save(Archive& archive, T& data)
    {
        constexpr auto I = meta::select(is_save_class<T>(), is_saveload_class<T>());
        using Mode = typename SaveMode<T>::template type<I>;

        Mode::call(archive, data);
    }

    template <typename Archive, typename T>
    static void load(Archive& archive, T& data)
    {
        constexpr auto I = meta::select(is_load_class<T>(), is_saveload_class<T>());
        using Mode = typename LoadMode<T>::template type<I>;

        Mode::call(archive, data);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(is_dynamic_save_class<T>() and
                        (is_save_class<T>() or is_saveload_class<T>()))>
    static void dynamic_save(core::ArchiveBase& archive, Pointer& object)
    {
        object->__save(archive);
    }

    template <typename Pointer, typename T = meta::dereference<Pointer>,
              SIREQUIRE(is_dynamic_load_class<T>() and
                        (is_load_class<T>() or is_saveload_class<T>()))>
    static void dynamic_load(core::ArchiveBase& archive, Pointer& object)
    {
        object->__load(archive);
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static auto trait(T& object) noexcept -> decltype(object.__trait())
    {
        return object.__trait();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static constexpr dynamic::PolymorphicTraitCore::key_type static_trait() noexcept
    {
        return T::__static_trait();
    }

    template <class T, SIREQUIRE(is_registered_class<T>())>
    static dynamic::PolymorphicTraitCore::key_type trait() noexcept
    {
        static constexpr auto trait_key = dynamic::PolymorphicTraitKey<T>::key;

        return trait_key == dynamic::PolymorphicTraitCore::base_key
             ? static_trait<T>()
             : trait_key;
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
    friend void base(Archive& archive, Derived& object)
    {
        archive & Access::template cast<Base&>(object);
    }
};

// Declaration friend function template for the Access class
template <typename Base, class Archive, typename Derived,
          meta::require<meta::is_archive<Archive>() and
                        meta::is_base_of<Base, Derived>()>>
void base(Archive& archive, Derived& object);

template <typename Base, class Archive, typename Derived,
          SIREQUIRE(meta::is_base_of<Base, Derived>())>
void virtual_base(Archive& archive, Derived& object)
{
    if (Access::trait(object) == Access::template trait<Derived>())
        base<Base>(archive, object);
}

namespace detail
{

template <class Base, class Archive, class Derived,
          SIREQUIRE(meta::can_static_cast<Base, Derived>() and
                    meta::is_base_of<Base, Derived>())>
void hierarchy_impl(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SIREQUIRE(not meta::can_static_cast<Base, Derived>() and
                    meta::is_base_of<Base, Derived>())>
void hierarchy_impl(Archive& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

_SIRAF_APPLY_FUNCTOR_GENERIC(BaseFunctor, base)
_SIRAF_APPLY_FUNCTOR_GENERIC(VirtualBaseFunctor, virtual_base)

} // namespace apply

_SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(BaseFunctor, base)
_SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC(VirtualBaseFunctor, virtual_base)

// Variadic native_base function
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) { /*pass*/ }

template <class Base, class... Base_n, class Archive, class Derived,
          SIREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>())>
void hierarchy(Archive& archive, Derived& object)
{
    detail::hierarchy_impl<Base>(archive, object);
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

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(Access::is_save_class<T>() or Access::is_load_class<T>())
CONDITIONAL_TYPE_REGISTRY(Access::is_saveload_class<T>())

// clean up
#undef _SIRAF_HAS_PROPERTY_HELPER
#undef _SIRAF_APPLY_FUNCTOR_GENERIC
#undef _SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC

#endif // SIRAF_ACCESS_HPP
