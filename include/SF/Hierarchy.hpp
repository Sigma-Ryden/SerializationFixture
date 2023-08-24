#ifndef SF_HIERARCHY_HPP
#define SF_HIERARCHY_HPP

#include <SF/Core/SerializatonBase.hpp>
#include <SF/Core/Serialization.hpp>
#include <SF/Core/Memory.hpp>

#include <SF/ApplyFunctor.hpp>
#include <SF/DataTrackBase.hpp>
#include <SF/HierarchyTrack.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <class Base, class Archive, class Derived,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              std::is_base_of<Base, Derived>>::value)>
void base(Archive& archive, Derived& object)
{
    ::Serialization::serialize_base<Base>(archive, object);
}

template <class Base, class Archive, class Derived,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              std::is_base_of<Base, Derived>>::value)>
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
          SFREQUIRE(not ::Serialization::is_virtual_base_of<Base, Derived>::value)>
void native_base(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SFREQUIRE(::Serialization::is_virtual_base_of<Base, Derived>::value)>
void native_base(Archive& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <class Derived, class Base>
struct BaseFunctor : ApplyFunctor
{
    Derived& object;

    BaseFunctor(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { base<Base>(archive, object); }
};

template <class Derived, class Base>
struct VirtualBaseFunctor : ApplyFunctor
{
    Derived& object;

    VirtualBaseFunctor(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { virtual_base<Base>(archive, object); }
};

} // namespace apply

template <class Base, class Derived,
          SFREQUIRE(std::is_base_of<Base, Derived>::value)>
apply::BaseFunctor<Derived, Base> base(Derived& object) noexcept { return { object }; }

template <class Base, class Derived,
          SFREQUIRE(std::is_base_of<Base, Derived>::value)>
apply::VirtualBaseFunctor<Derived, Base> virtual_base(Derived& object) noexcept { return { object }; }

// default empty implementation
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) noexcept { /*pass*/ }

// Variadic native_base function
template <class Base, class... Base_n, class Archive, class Derived,
          SFREQUIRE(meta::all<meta::is_ioarchive<Archive>,
                              meta::is_derived_of<Derived, Base, Base_n...>>::value)>
void hierarchy(Archive& archive, Derived& object)
{
    detail::native_base<Base>(archive, object);
    hierarchy<Base_n...>(archive, object);
}

namespace apply
{

template <class Derived, class Base, class... Base_n>
struct HierarchyFunctor : ApplyFunctor
{
    Derived& object;

    HierarchyFunctor(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { hierarchy<Base, Base_n...>(archive, object); }
};

} // namespace apply

template <class Base, class... Base_n, class Derived,
          SFREQUIRE(meta::is_derived_of<Derived, Base, Base_n...>::value)>
apply::HierarchyFunctor<Derived, Base, Base_n...> hierarchy(Derived& object) noexcept
{
    return { object };
}

} // namespace sf

#endif // SF_HIERARCHY_HPP
