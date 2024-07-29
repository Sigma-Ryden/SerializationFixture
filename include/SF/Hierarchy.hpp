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
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               std::is_base_of<Base, Derived>>::value)>
void base(Archive& archive, Derived& object)
{
    ::xxsf::serialize_base<Base>(archive, object);
}

template <class Base, class Archive, class Derived,
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               std::is_base_of<Base, Derived>>::value)>
void virtual_base(Archive& archive, Derived& object)
{
#ifdef SF_PTRTRACK_DISABLE
    if (::xxsf::traits(object) == ::xxsf::template traits<Derived>())
        base<Base>(archive, object);
#else
    using key_type = typename Archive::TrackingKeyType;

    auto address = memory::pure(std::addressof(object));

    const auto key = reinterpret_cast<key_type>(address);
    const auto traits = ::xxsf::traits<Base>();

    auto& hierarchy_tracking = archive.template tracking<tracking::hierarchy_t>();

    auto& is_tracking = hierarchy_tracking[{key, traits}];
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
          SF_REQUIRE(not meta::is_virtual_base_of<Base, Derived>::value)>
void native_base(Archive& archive, Derived& object_with_base)
{
    base<Base>(archive, object_with_base);
}

template <class Base, class Archive, class Derived,
          SF_REQUIRE(meta::is_virtual_base_of<Base, Derived>::value)>
void native_base(Archive& archive, Derived& object_with_virtual_base)
{
    virtual_base<Base>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <class Derived, class Base>
struct base_functor_t : apply_functor_t
{
    Derived& object;

    base_functor_t(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { base<Base>(archive, object); }
};

template <class Derived, class Base>
struct virtual_base_functor_t : apply_functor_t
{
    Derived& object;

    virtual_base_functor_t(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { virtual_base<Base>(archive, object); }
};

} // namespace apply

template <class Base, class Derived,
          SF_REQUIRE(std::is_base_of<Base, Derived>::value)>
apply::base_functor_t<Derived, Base> base(Derived& object) noexcept { return { object }; }

template <class Base, class Derived,
          SF_REQUIRE(std::is_base_of<Base, Derived>::value)>
apply::virtual_base_functor_t<Derived, Base> virtual_base(Derived& object) noexcept { return { object }; }

// default empty impl
template <class Archive, class Derived>
void hierarchy(Archive& archive, Derived& object) noexcept { /*pass*/ }

// Variadic native_base function
template <class Base, class... Base_n, class Archive, class Derived,
          SF_REQUIRE(meta::all<meta::is_ioarchive<Archive>,
                               meta::is_derived_of<Derived, Base, Base_n...>>::value)>
void hierarchy(Archive& archive, Derived& object)
{
    detail::native_base<Base>(archive, object);
    hierarchy<Base_n...>(archive, object);
}

namespace apply
{

template <class Derived, class Base, class... Base_n>
struct hierarchy_functor_t : apply_functor_t
{
    Derived& object;

    hierarchy_functor_t(Derived& object) noexcept : object(object) {}

    template <class Archive>
    void operator() (Archive& archive) const { hierarchy<Base, Base_n...>(archive, object); }
};

} // namespace apply

template <class Base, class... Base_n, class Derived,
          SF_REQUIRE(meta::is_derived_of<Derived, Base, Base_n...>::value)>
apply::hierarchy_functor_t<Derived, Base, Base_n...> hierarchy(Derived& object) noexcept
{
    return { object };
}

} // namespace sf

#endif // SF_HIERARCHY_HPP
