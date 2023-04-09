#ifndef SF_HIERARCHY_HPP
#define SF_HIERARCHY_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/ApplyFunctor.hpp>
#include <SF/DataTrackBase.hpp>
#include <SF/HierarchyTrack.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <typename Base, class Archive, typename Derived,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_base_of<Base, Derived>())>
void base(Archive& archive, Derived& object)
{
    ::Serialization::serialize_base<Base>(archive, object);
}

template <typename Base, class Archive, typename Derived,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_base_of<Base, Derived>())>
void virtual_base(Archive& archive, Derived& object)
{
#ifdef SF_PTRTRACK_DISABLE
    if (::Serialization::trait(object) == ::Serialization::template trait<Derived>())
        base<Base>(archive, object);
#else
    using key_type = typename Archive::TrackingKeyType;

    auto address = Memory::pure(std::addressof(object));

    auto key = reinterpret_cast<key_type>(address);
    auto trait = ::Serialization::trait<Base>();

    auto& hierarchy_tracking = archive.template tracking<tracking::Hierarchy>();

    auto& is_tracking = hierarchy_tracking[{key, trait}];
    if (not is_tracking)
    {
        is_tracking = true;
        base<Base>(archive, object);
    }
#endif // SF_PTRTRACK_DISABLE
}

namespace detail
{

template <class Base, class Archive, class Derived,
          SIREQUIRE(::Serialization::is_virtual_base_of<Base, Derived>())>
void native_base(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SIREQUIRE(not ::Serialization::is_virtual_base_of<Base, Derived>())>
void native_base(Archive& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <typename Derived, typename Base>
struct BaseFunctor : ApplyFunctor
{
    Derived& object;

    template <class Archive>
    void operator() (Archive& archive) { base<Base>(archive, object); }
};

template <typename Derived, typename Base>
struct VirtualBaseFunctor : ApplyFunctor
{
    Derived& object;

    template <class Archive>
    void operator() (Archive& archive) { virtual_base<Base>(archive, object); }
};

} // namespace apply

template <typename Base, typename Derived,
          SIREQUIRE(meta::is_base_of<Base, Derived>())>
apply::BaseFunctor<Derived, Base> base(Derived& object) { return { {}, object }; }

template <typename Base, typename Derived,
          SIREQUIRE(meta::is_base_of<Base, Derived>())>
apply::VirtualBaseFunctor<Derived, Base> virtual_base(Derived& object) { return { {}, object }; }

// default empty impl
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) noexcept { /*pass*/ }

// Variadic native_base function
template <class Base, class... Base_n, class Archive, class Derived,
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_derived_of<Derived, Base, Base_n...>())>
void hierarchy(Archive& archive, Derived& object)
{
    detail::native_base<Base>(archive, object);
    hierarchy<Base_n...>(archive, object);
}

namespace apply
{

template <typename Derived, typename Base, typename... Base_n>
struct HierarchyFunctor : ApplyFunctor
{
    Derived& object;

    template <typename Archive>
    void operator() (Archive& archive) { hierarchy<Base, Base_n...>(archive, object); }
};

} // namespace apply

template <class Base, class... Base_n, class Derived,
          SIREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>())>
apply::HierarchyFunctor<Derived, Base, Base_n...> hierarchy(Derived& object)
{
    return { {}, object };
}

} // namespace sf

#endif // SF_HIERARCHY_HPP
