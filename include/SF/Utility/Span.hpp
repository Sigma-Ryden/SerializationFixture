#ifndef SF_UTILITY_SPAN_HPP
#define SF_UTILITY_SPAN_HPP

#include <cstddef> // size_t
#include <tuple> // tuple

#include <SF/Core/SerializatonBase.hpp>

#include <SF/Utility/Alias.hpp>

#include <SF/ApplyFunctor.hpp>
#include <SF/Strict.hpp>

#include <SF/Detail/Meta.hpp>
#include <SF/Detail/MetaMacro.hpp>

namespace sf
{

namespace utility
{

template <typename T, std::size_t N>
class span_t;

template <typename T, std::size_t N>
class span_base_t
{
public:
    using size_type         = std::size_t;
    using value_type        = span_t<T, N - 1>;

    using pointer           = typename meta::pointer<T, N>::type;
    using const_pointer     = const pointer;

protected:
    using Dimension         = size_type[N];

protected:
    alias_t<pointer> data_;
    Dimension dim_;

protected:
    span_base_t(pointer& data) noexcept
        : data_(data), dim_() {}

    template <typename D, typename... Dn>
    span_base_t(pointer& data, D d, Dn... dn) noexcept
        : data_(data), dim_{d, dn...} {}

public:
    void init(pointer data) noexcept { data_.get() = data; }
    void data(alias_t<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    size_type size() const noexcept { return dim_[0]; }
    Dimension& dim() noexcept { return dim_; }
};

template <typename T, std::size_t N>
class span_t : public span_base_t<T, N>
{
protected:
    using Core = span_base_t<T, N>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = span_t<T, N - 1>;
    using dereference_type  = typename meta::pointer<T, N - 1>::type;

    using reference         = value_type&;
    using const_reference   = const value_type&;

protected:
    using typename Core::Dimension;

private:
    mutable value_type child_scope_;

public:
    span_t(pointer& data, Dimension dim) noexcept
        : Core(data), child_scope_(data[0], dim + 1)
    {
        for (size_type i = 0; i < N; ++i)
            this->dim_[i] = dim;
    }

    template <typename D, typename... Dn,
              SF_REQUIRE(not std::is_array<D>::value)>
    span_t(pointer& data, D d, Dn... dn) noexcept
        : Core(data, d, dn...), child_scope_(data[0], dn...) {}

    void size(size_type value) noexcept { this->dim_[0] = value; }

    reference operator[] (size_type i) noexcept
    {
        child_scope_.data(this->data_[i]);
        return child_scope_;
    }

public:
    using Core::size; // prevent Core function hiding
};

template <typename T>
class span_t<T, 1> : public span_base_t<T, 1>
{
protected:
    using Core = span_base_t<T, 1>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = T;
    using dereference_type  = T;

    using reference         = T&;
    using const_reference   = const T&;

protected:
    using typename Core::Dimension;

public:
    span_t(pointer& data, Dimension size) noexcept
        : Core(data)
    {
        this->dim_[0] = size[0];
    }

    span_t(pointer& data, size_type size) noexcept
        : Core(data, size) {}

    reference operator[] (size_type i) noexcept { return this->data_[i]; }
};

} // namespace utility

namespace meta
{

template <typename> struct is_span : std::false_type {};
template <typename T, std::size_t N>
struct is_span<utility::span_t<T, N>> : std::true_type {};

template <typename Pointer, typename D, typename... Dn>
struct is_span_set
    : meta::all<std::integral_constant<bool, meta::pointer_count<Pointer>::value >= sizeof...(Dn) + 1>,
                meta::all<std::is_arithmetic<D>,
                          std::is_arithmetic<Dn>...>> {};

} // namespace meta

namespace utility
{

template <typename Pointer, typename D, typename... Dn,
          std::size_t N = sizeof...(Dn) + 1,
          typename Type = typename meta::remove_pointer<Pointer, N>::type,
          SF_REQUIRE(meta::is_span_set<Pointer, D, Dn...>::value)>
span_t<Type, N> make_span(Pointer& data, D d, Dn... dn)
{
    using size_type = typename span_t<Type, N>::size_type;
    return { data, static_cast<size_type>(d), static_cast<size_type>(dn)... };
}

} // namespace utility

namespace detail
{

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::negation<meta::is_span<T>>>::value)>
void span_impl(ArchiveType& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_oarchive<ArchiveType>,
                               meta::is_span<T>>::value)>
void span_impl(ArchiveType& archive, T& array)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

template <class ArchiveType, typename T,
          SF_REQUIRE(meta::all<meta::is_iarchive<ArchiveType>,
                               meta::is_span<T>>::value)>
void span_impl(ArchiveType& archive, T& array)
{
    using size_type = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    auto ptr = new dereference_type [array.size()] {};
    array.init(ptr);

    for (size_type i = 0; i < array.size(); ++i)
        span_impl(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class ArchiveType, typename T,
          typename D, typename... Dn,
          SF_REQUIRE(meta::all<meta::is_ioarchive<ArchiveType>,
                               meta::is_span_set<T, D, Dn...>>::value)>
void span(ArchiveType& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (not detail::refer_key(archive, pointer)) return; // serialize refer info
    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::span_impl(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename T, typename D, typename... Dn>
struct span_functor_t : apply_functor_t
{
    using PackType = std::tuple<T&, D&, Dn&...>;

    PackType pack;

    span_functor_t(T& pointer, D& d, Dn&... dn) noexcept : pack(pointer, d, dn...) {}

    template <class ArchiveType>
    void operator() (ArchiveType& archive) const
    {
        invoke(archive, meta::make_index_sequence<std::tuple_size<PackType>::value>{});
    }

private:
    template <class ArchiveType, std::size_t... I>
    void invoke(ArchiveType& archive, meta::index_sequence<I...>) const
    {
        span(archive, std::get<I>(pack)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename T, typename D, typename... Dn,
          SF_REQUIRE(meta::is_span_set<T, D, Dn...>::value)>
apply::span_functor_t<T, D, Dn...> span(T& pointer, D& dimension, Dn&... dimension_n) noexcept
{
    return { pointer, dimension, dimension_n... };
}

} // inline namespace common

} // namespace sf

#endif // SF_UTILITY_SPAN_HPP
