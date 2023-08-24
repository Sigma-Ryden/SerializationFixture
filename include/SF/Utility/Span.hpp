#ifndef SF_UTILITY_SPAN_HPP
#define SF_UTILITY_SPAN_HPP

#include <cstddef> // size_t
#include <initializer_list> // initializer_list
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
class Span;

template <typename T, std::size_t N>
class SpanBase
{
public:
    using size_type         = std::size_t;
    using value_type        = Span<T, N - 1>;

    using pointer           = meta::pointer<T, N>;
    using const_pointer     = const meta::pointer<T, N>;

protected:
    using Dimension         = size_type[N];

protected:
    alias<pointer> data_;
    Dimension dim_;

protected:
    SpanBase(pointer& data) noexcept
        : data_(data), dim_() {}

    template <typename D, typename... Dn>
    SpanBase(pointer& data, D d, Dn... dn) noexcept
        : data_(data), dim_{d, dn...} {}

public:
    void init(pointer data) noexcept { data_.get() = data; }
    void data(alias<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    size_type size() const noexcept { return dim_[0]; }
    Dimension& dim() noexcept { return dim_; }
};

template <typename T, std::size_t N>
class Span : public SpanBase<T, N>
{
protected:
    using Core = SpanBase<T, N>;

public:
    using typename Core::size_type;
    using typename Core::pointer;

public:
    using value_type        = Span<T, N - 1>;
    using dereference_type  = meta::pointer<T, N - 1>;

    using reference         = value_type&;
    using const_reference   = const value_type&;

protected:
    using typename Core::Dimension;

private:
    mutable value_type child_scope_;

public:
    Span(pointer& data, Dimension dim) noexcept
        : Core(data), child_scope_(data[0], dim + 1)
    {
        for (size_type i = 0; i < N; ++i)
            this->dim_[i] = dim;
    }

    template <typename D, typename... Dn,
              SFREQUIRE(not std::is_array<D>::value)>
    Span(pointer& data, D d, Dn... dn) noexcept
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
class Span<T, 1> : public SpanBase<T, 1>
{
protected:
    using Core = SpanBase<T, 1>;

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
    Span(pointer& data, Dimension size) noexcept
        : Core(data)
    {
        this->dim_[0] = size[0];
    }

    Span(pointer& data, size_type size) noexcept
        : Core(data, size) {}

    reference operator[] (size_type i) noexcept { return this->data_[i]; }
};

} // namespace utility

namespace meta
{

template <typename> struct is_span : std::false_type {};
template <typename T, std::size_t N>
struct is_span<utility::Span<T, N>> : std::true_type {};

template <typename Pointer, typename D, typename... Dn>
struct is_span_set
    : meta::all<meta::boolean<meta::pointer_count<Pointer>() >= sizeof...(Dn) + 1>,
                meta::all<std::is_arithmetic<D>,
                          std::is_arithmetic<Dn>...>> {};

} // namespace meta

namespace utility
{

template <typename Pointer, typename D, typename... Dn,
          std::size_t N = sizeof...(Dn) + 1,
          typename Type = meta::remove_pointer<Pointer, N>,
          SFREQUIRE(meta::is_span_set<Pointer, D, Dn...>::value)>
Span<Type, N> make_span(Pointer& data, D d, Dn... dn)
{
    using size_type = typename Span<Type, N>::size_type;
    return { data, static_cast<size_type>(d), static_cast<size_type>(dn)... };
}

} // namespace utility

namespace detail
{

template <class Archive, typename T,
          SFREQUIRE(meta::all<meta::is_archive<Archive>,
                              meta::negation<meta::is_span<T>>>::value)>
void span_implementation(Archive& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class OArchive, typename T,
          SFREQUIRE(meta::all<meta::is_oarchive<OArchive>,
                              meta::is_span<T>>::value)>
void span_implementation(OArchive& archive, T& array)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        span_implementation(archive, array[i]);
}

template <class IArchive, typename T,
          SFREQUIRE(meta::all<meta::is_iarchive<IArchive>,
                              meta::is_span<T>>::value)>
void span_implementation(IArchive& archive, T& array)
{
    using size_type        = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    using pointer          = typename T::pointer;

    auto ptr = new dereference_type [array.size()] {};
    array.init(ptr);

    for (size_type i = 0; i < array.size(); ++i)
        span_implementation(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class Archive, typename T,
          typename D, typename... Dn,
          SFREQUIRE(meta::all<meta::is_archive<Archive>,
                              meta::is_span_set<T, D, Dn...>>::value)>
void span(Archive& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (not detail::refer_key(archive, pointer)) return; // serialize refer info
    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::span_implementation(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename T, typename D, typename... Dn>
struct SpanFunctor : ApplyFunctor
{
    using Pack = std::tuple<T&, D&, Dn&...>;

    Pack pack;

    SpanFunctor(T& pointer, D& d, Dn&... dn) noexcept : pack(pointer, d, dn...) {}

    template <class Archive>
    void operator() (Archive& archive) const
    {
        invoke(archive, meta::make_index_sequence<std::tuple_size<Pack>::value>{});
    }

private:
    template <class Archive, std::size_t... I>
    void invoke(Archive& archive, meta::index_sequence<I...>) const
    {
        span(archive, std::get<I>(pack)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename T, typename D, typename... Dn,
          SFREQUIRE(meta::is_span_set<T, D, Dn...>::value)>
apply::SpanFunctor<T, D, Dn...> span(T& pointer, D& dimension, Dn&... dimension_n) noexcept
{
    return { pointer, dimension, dimension_n... };
}

} // inline namespace common

} // namespace sf

#endif // SF_UTILITY_SPAN_HPP
