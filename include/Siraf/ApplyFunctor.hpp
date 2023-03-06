#ifndef SIRAF_APPLY_FUNCTOR_HPP
#define SIRAF_APPLY_FUNCTOR_HPP

#include <Siraf/Core/TypeRegistry.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
{

namespace apply
{

struct ApplyFunctor {};

} // namespace apply

namespace meta
{

template <typename T> constexpr bool is_apply_functor() noexcept
{
    return std::is_base_of<apply::ApplyFunctor, T>::value;
}

} // namespace meta

// inline namespace common also used in namespace library
inline namespace common
{

template <typename Archive, typename T,
          typename dT = meta::decay<T>, // T can be lvalue
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_registered<dT>() and
                    meta::is_apply_functor<dT>())>
Archive& operator& (Archive& archive, T&& apply_functor)
{
    apply_functor(archive);
    return archive;
}

} // inline namespace common

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_apply_functor<T>())

#endif // SIRAF_APPLY_FUNCTOR_HPP
