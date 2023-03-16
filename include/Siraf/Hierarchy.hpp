#ifndef SIRAF_HIERARCHY_HPP
#define SIRAF_HIERARCHY_HPP

#include <Siraf/Core/Access.hpp>
#include <Siraf/ApplyFunctor.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

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

// Declaration friend function template for the Access class
template <typename Base, class Archive, typename Derived,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_base_of<Base, Derived>())>
void base(Archive& archive, Derived& object)
{
    core::Access::serialize_base<Base>(archive, object);
}

template <typename Core, class Archive, typename Derived,
          SIREQUIRE(meta::is_base_of<Core, Derived>())>
void virtual_base(Archive& archive, Derived& object)
{
    if (core::Access::trait(object) == core::Access::template trait<Derived>())
        base<Core>(archive, object);
}

namespace detail
{

template <class Base, class Archive, class Derived,
          SIREQUIRE(core::Access::can_static_cast<Base*, Derived*>() and
                    meta::is_base_of<Base, Derived>())>
void native_base(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SIREQUIRE(not core::Access::can_static_cast<Base*, Derived*>() and
                    meta::is_base_of<Base, Derived>())>
void native_base(Archive& archive, Derived& object_with_virtual_base)
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

// default empty impl
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) noexcept { /*pass*/ }

// Variadic native_base function
template <class Base, class... Base_n, class Archive, class Derived,
          SIREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>())>
void hierarchy(Archive& archive, Derived& object)
{
    detail::native_base<Base>(archive, object);
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

// clean up
#undef _SIRAF_APPLY_FUNCTOR_GENERIC
#undef _SIRAF_APPLY_FUNCTOR_FACTORY_FUNCTION_GENERIC

#endif // SIRAF_HIERARCHY_HPP
