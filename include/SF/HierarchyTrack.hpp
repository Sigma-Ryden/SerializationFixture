#ifndef SF_HIERARCHY_TRACK_HPP
#define SF_HIERARCHY_TRACK_HPP

#include <SF/Core/ArchiveBase.hpp>
#include <SF/Core/Memory.hpp>
#include <SF/Core/Hash.hpp>

#include <SF/ApplyFunctor.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       std::is_base_of<BaseType, DerivedType>>::value)>
void base(ArchiveType& archive, DerivedType& object)
{
    archive & ::xxsf_cast_to_non_public_base::call<BaseType>(object);
}

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       std::is_base_of<BaseType, DerivedType>>::value)>
void virtual_base(ArchiveType& archive, DerivedType& object)
{
#ifdef SF_PTRTRACK_DISABLE
    if (SF_EXPRESSION_HASH(object) == SF_TYPE_HASH(DerivedType))
        base<Base>(archive, object);
#else
    auto address = memory::pure(std::addressof(object));

    auto const key = reinterpret_cast<std::uintptr_t>(address);
    auto const traits = SF_TYPE_HASH(BaseType);

    auto& is_tracking = archive.tracking().hierarchy()[key][traits];
    if (not is_tracking)
    {
        is_tracking = true;
        base<BaseType>(archive, object);
    }
#endif // SF_PTRTRACK_DISABLE
}

namespace detail
{

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(std::negation<meta::is_virtual_base_of<BaseType, DerivedType>>::value)>
void native_base(ArchiveType& archive, DerivedType& object_with_base)
{
    base<BaseType>(archive, object_with_base);
}

template <class BaseType, class ArchiveType, class DerivedType,
          SF_REQUIRES(meta::is_virtual_base_of<BaseType, DerivedType>::value)>
void native_base(ArchiveType& archive, DerivedType& object_with_virtual_base)
{
    virtual_base<BaseType>(archive, object_with_virtual_base);
}

} // namespace detail

namespace apply
{

template <class DerivedType, class BaseType>
struct base_functor_t : apply_functor_t
{
    DerivedType& object;

    base_functor_t(DerivedType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { base<BaseType>(archive, object); }
};

template <class DerivedType, class BaseType>
struct virtual_base_functor_t : apply_functor_t
{
    DerivedType& object;

    virtual_base_functor_t(DerivedType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { virtual_base<BaseType>(archive, object); }
};

} // namespace apply

template <class BaseType, class DerivedType,
          SF_REQUIRES(std::is_base_of<BaseType, DerivedType>::value)>
apply::base_functor_t<DerivedType, BaseType> base(DerivedType& object) noexcept { return { object }; }

template <class BaseType, class DerivedType,
          SF_REQUIRES(std::is_base_of<BaseType, DerivedType>::value)>
apply::virtual_base_functor_t<DerivedType, BaseType> virtual_base(DerivedType& object) noexcept { return { object }; }

// default empty impl
template <class ArchiveType, class DerivedType>
void hierarchy(ArchiveType&, DerivedType&) noexcept { /*pass*/ }

// Variadic native_base function
template <class BaseType, class... BaseTypes, class ArchiveType, class DerivedType,
          SF_REQUIRES(std::conjunction<meta::is_ioarchive<ArchiveType>,
                                       meta::is_derived_of<DerivedType, BaseType, BaseTypes...>>::value)>
void hierarchy(ArchiveType& archive, DerivedType& object)
{
    detail::native_base<BaseType>(archive, object);
    hierarchy<BaseTypes...>(archive, object);
}

namespace apply
{

template <class DerivedType, class BaseType, class... BaseTypes>
struct hierarchy_functor_t : apply_functor_t
{
    DerivedType& object;

    hierarchy_functor_t(DerivedType& object) noexcept : object(object) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const { hierarchy<BaseType, BaseTypes...>(archive, object); }
};

} // namespace apply

template <class BaseType, class... BaseTypes, class DerivedType,
          SF_REQUIRES(meta::is_derived_of<DerivedType, BaseType, BaseTypes...>::value)>
apply::hierarchy_functor_t<DerivedType, BaseType, BaseTypes...> hierarchy(DerivedType& object) noexcept
{
    return { object };
}

} // namespace sf

#endif // SF_HIERARCHY_TRACK_HPP
