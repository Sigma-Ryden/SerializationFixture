#ifndef SIFAR_APPLY_FUNCTOR_HPP
#define SIFAR_APPLY_FUNCTOR_HPP

#include <Sifar/TypeRegistry.hpp>

#include <Sifar/Detail/Meta.hpp>
#include <Sifar/Detail/MetaMacro.hpp>

namespace sifar
{

namespace apply
{

struct ApplyFunctor {};

} // namespace apply

namespace meta
{

template <typename T> constexpr bool is_apply_functor() noexcept
{
    return meta::is_base_of<apply::ApplyFunctor, T>();
}

} // namespace meta

template <typename Archive, typename T,
          typename U = meta::decay<T>, // T can be lvalue
          SIREQUIRE(meta::is_archive<Archive>() and
                    meta::is_registered<U>() and
                    meta::is_apply_functor<U>())>
Archive& operator& (Archive& archive, T&& apply_functor)
{
    apply_functor(archive);
    return archive;
}

} // namespace sifar

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_apply_functor<T>())

#endif // SIFAR_APPLY_FUNCTOR_HPP
