#ifndef SIRAF_UTILITY_SPAN_HPP
#define SIRAF_UTILITY_SPAN_HPP

#include <cstddef> // size_t
#include <initializer_list> // initializer_list
#include <tuple> // tuple

#include <Siraf/Utility/Ref.hpp>
#include <Siraf/ApplyFunctor.hpp>

#include <Siraf/Detail/Meta.hpp>
#include <Siraf/Detail/MetaMacro.hpp>

namespace siraf
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
    Ref<pointer> data_;
    Dimension dim_;

protected:
    SpanBase(pointer& data) noexcept;

    template <typename D, typename... Dn>
    SpanBase(pointer& data, D d, Dn... dn) noexcept;

public:
    void init(pointer data) noexcept { data_.get() = data; }
    void data(Ref<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    const_pointer& data() const noexcept { return data_; }

    size_type size() const noexcept { return dim_[0]; }

    Dimension& dim() noexcept { return dim_; }
    const Dimension& dim() const noexcept { return dim_; }
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
    Span(pointer& data, Dimension dim_) noexcept;

    template <typename D, typename... Dn,
              meta::require<not meta::is_array<D>()> = 0>
    Span(pointer& data, D d, Dn... dn) noexcept;

    void size(size_type value) noexcept { this->dim_[0] = value; }

    reference operator[] (size_type i) noexcept;
    const_reference operator[] (size_type i) const noexcept;

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
    Span(pointer& data, Dimension size) noexcept;
    Span(pointer& data, size_type size) noexcept;

    reference operator[] (size_type i) noexcept { return this->data_[i]; }
    const_reference operator[] (size_type i) const noexcept { return this->data_[i]; }
};

template <typename T, std::size_t N>
inline SpanBase<T, N>::SpanBase(pointer& data) noexcept
    : data_(data)
    , dim_()
{
}

template <typename T, std::size_t N>
template <typename D, typename... Dn>
SpanBase<T, N>::SpanBase(pointer& data, D d, Dn... dn) noexcept
    : data_(data)
    , dim_{d, dn...}
{
}

template <typename T, std::size_t N>
Span<T, N>::Span(pointer& data, Dimension dim) noexcept
    : Core(data)
    , child_scope_(data[0], dim + 1)
{
    for (size_type i = 0; i < N; ++i)
        this->dim_[i] = dim;
}

template <typename T, std::size_t N>
template <typename D, typename... Dn,
          meta::require<not meta::is_array<D>()>>
Span<T, N>::Span(pointer& data, D d, Dn... dn) noexcept
    : Core(data, d, dn...)
    , child_scope_(data[0], dn...)
{
}

template <typename T, std::size_t N>
inline auto Span<T, N>::operator[] (size_type i) noexcept -> reference
{
    child_scope_.data(this->data_[i]);
    return child_scope_;
}

template <typename T, std::size_t N>
inline auto Span<T, N>::operator[] (size_type i) const noexcept -> const_reference
{
    child_scope_.data(this->data_[i]);
    return child_scope_;
}

template <typename T>
inline Span<T, 1>::Span(pointer& data, Dimension dim) noexcept
    : Core(data)
{
    this->dim_[0] = dim[0];
}

template <typename T>
inline Span<T, 1>::Span(pointer& data, size_type size) noexcept
    : Core(data, size)
{
}

} // namespace utility

namespace meta
{

namespace detail
{

template <typename> struct is_span : std::false_type {};
template <typename T, std::size_t N>
struct is_span<utility::Span<T, N>> : std::true_type {};

} // namespcae detail

template <class T> constexpr bool is_span() noexcept
{
    return detail::is_span<T>::value;
}

template <typename Pointer, typename D, typename... Dn,
          std::size_t DimentionSize = sizeof...(Dn) + 1>
constexpr bool is_span_set() noexcept
{
    return meta::pointer_count<Pointer>() >= DimentionSize
       and meta::all<std::is_arithmetic<D>,
                     std::is_arithmetic<Dn>...>();
}

} // namespace meta

namespace utility
{

template <typename Pointer, typename D, typename... Dn,
          std::size_t N = sizeof...(Dn) + 1,
          typename Type = meta::remove_ptr_n<Pointer, N>,
          SIREQUIRE(meta::is_span_set<Pointer, D, Dn...>())>
Span<Type, N> make_span(Pointer& data, D d, Dn... dn)
{
    using size_type = typename Span<Type, N>::size_type;
    return { data, static_cast<size_type>(d), static_cast<size_type>(dn)... };
}

} // namespace utility

namespace detail
{

template <class Archive, typename T,
          SIREQUIRE(meta::is_archive<Archive>() and not meta::is_span<T>())>
void raw_span(Archive& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class WriteArchive, typename T,
          SIREQUIRE(meta::is_write_archive<WriteArchive>()
                    and meta::is_span<T>())>
void raw_span(WriteArchive& archive, T& array)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        raw_span(archive, array[i]);
}

template <class ReadArchive, typename T,
          SIREQUIRE(meta::is_read_archive<ReadArchive>()
                    and meta::is_span<T>())>
void raw_span(ReadArchive& archive, T& array)
{
    using size_type        = typename T::size_type;
    using dereference_type = typename T::dereference_type;

    using pointer          = typename T::pointer;

    pointer ptr = new dereference_type [array.size()];
    array.init(ptr);

    for (size_type i = 0; i < array.size(); ++i)
        raw_span(archive, array[i]);
}

} // namespace detail

// inline namespace common also used in namespace library
inline namespace common
{

template <class WriteArchive, typename T,
          typename D, typename... Dn,
          SIREQUIRE(meta::is_write_archive<WriteArchive>() and
                    meta::is_span_set<T, D, Dn...>())>
void span(WriteArchive& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (pointer == nullptr)
        throw "The write span data must be allocated.";

    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::raw_span(archive, span_data);
}

template <class ReadArchive, typename T,
          typename D, typename... Dn,
          SIREQUIRE(meta::is_read_archive<ReadArchive>() and
                    meta::is_span_set<T, D, Dn...>())>
void span(ReadArchive& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (pointer != nullptr)
        throw "The read span data must be initialized to nullptr.";

    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::raw_span(archive, span_data);
}

} // inline namespace common

namespace apply
{

template <typename T, typename D, typename... Dn>
class SpanFunctor : public ApplyFunctor
{
private:
    using Pack = std::tuple<T&, D&, Dn&...>;

    Pack pack_;

public:
    SpanFunctor(T& pointer, D& dimension, Dn&... dimension_n) noexcept
        : pack_(pointer, dimension, dimension_n...) {}

    template <typename Archive,
              SIREQUIRE(meta::is_archive<Archive>())>
    void operator() (Archive& archive)
    {
        invoke(archive, meta::make_index_sequence<std::tuple_size<Pack>::value>{});
    }

private:
    template <typename Archive, std::size_t... I>
    void invoke(Archive& archive, meta::index_sequence<I...>)
    {
        span(archive, std::get<I>(pack_)...);
    }
};

} // namespace apply

// inline namespace common also used in namespace library
inline namespace common
{

template <typename T, typename D, typename... Dn,
          SIREQUIRE(meta::is_span_set<T, D, Dn...>())>
apply::SpanFunctor<T, D, Dn...> span(T& pointer, D& dimension, Dn&... dimension_n)
{
    return { pointer, dimension, dimension_n... };
}

} // inline namespace common

} // namespace siraf

#endif // SIRAF_UTILITY_SPAN_HPP
