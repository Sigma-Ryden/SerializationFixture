#ifndef SIFAR_DETAIL_POINTER_HPP
#define SIFAR_DETAIL_POINTER_HPP

#include <cstddef> // size_t

namespace sifar
{

namespace meta
{

namespace detail
{

template <typename T, std::size_t N>
struct pointer_impl
{
    using type = typename pointer_impl<T, N - 1>::type*;
};

template <typename T>
struct pointer_impl<T, 0>
{
    using type = T;
};

} // namespace detail

template <typename T, std::size_t N>
using pointer = typename detail::pointer_impl<T, N>::type;

} // namespace meta

} // namespace sifar

#endif // SIFAR_DETAIL_POINTER_HPP
