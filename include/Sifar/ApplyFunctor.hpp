#ifndef SIFAR_APPLY_FUNCTOR_HPP
#define SIFAR_APPLY_FUNCTOR_HPP

#include <Sifar/Detail/Meta.hpp>

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
          meta::require<meta::is_archive<Archive>() and
                        meta::is_apply_functor<T>()> = 0>
Archive& operator& (Archive& archive, T&& apply_functor)
{
    apply_functor(archive);

    return archive;
}

} // namespace sifar

#endif // SIFAR_APPLY_FUNCTOR_HPP
