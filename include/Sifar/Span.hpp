#ifndef SIFAR_SPAN_HPP
#define SIFAR_SPAN_HPP

#include <cstddef> // size_t
#include <initializer_list> // initializer_list
#include <tuple> // tuple

#include <Sifar/Ref.hpp>

#include <Sifar/Utility.hpp>
#include <Sifar/ApplyFunctor.hpp>

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace utility
{

template <typename T, std::size_t N>
class Span
{
public:
    using size_type         = std::size_t;
    using value_type        = Span<T, N - 1>;
    using dereference_type  = meta::pointer<T, N - 1>;

    using reference         = value_type&;
    using const_reference   = const value_type&;

    using pointer           = meta::pointer<T, N>;
    using const_pointer     = const meta::pointer<T, N>;

private:
    using Dimension         = size_type[N];

private:
    Ref<pointer> data_;
    Dimension dim_;

    mutable value_type child_scope_;

public:
    Span(pointer& data, Dimension dim_);

    template <typename D, typename... Dn,
              meta::require<not meta::is_array<D>()> = 0>
    Span(pointer& data, D d, Dn... dn);

    void init(pointer data) noexcept { data_.get() = data; }
    void data(Ref<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    const_pointer& data() const noexcept { return data_; }

    Dimension& dim() noexcept { return dim_; }
    const Dimension& dim() const noexcept { return dim_; }

    size_type size() const noexcept { return dim_[0]; }
    void size(size_type value) noexcept { dim_[0] = value; }

    reference operator[] (size_type i) noexcept;
    const_reference operator[] (size_type i) const noexcept;
};

template <typename T>
class Span<T, 1>
{
public:
    using size_type         = std::size_t;
    using value_type        = T;
    using dereference_type  = T;

    using reference         = T&;
    using const_reference   = const T&;

    using pointer           = meta::pointer<T, 1>;
    using const_pointer     = const meta::pointer<T, 1>;

    using Dimension         = size_type[1];

private:
    Ref<pointer> data_;
    Dimension dim_;

public:
    Span(pointer& data, Dimension size);
    Span(pointer& data, size_type size);

    void init(pointer data) noexcept { data_.get() = data; }
    void data(Ref<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_; }
    const_pointer& data() const noexcept { return data_; }

    size_type size() const noexcept { return dim_[0]; }

    Dimension& dim() noexcept { return dim_; }
    const Dimension& dim() const noexcept { return dim_; }

    reference operator[] (size_type i) noexcept { return data_[i]; }
    const_reference operator[] (size_type i) const noexcept { return data_[i]; }
};

template <typename T, std::size_t N>
Span<T, N>::Span(pointer& data, Dimension dim)
    : data_(data)
    , dim_()
    , child_scope_(data[0], dim + 1)
{
    for (size_type i = 0; i < N; ++i)
        dim_[i] = dim;
}

template <typename T, std::size_t N>
template <typename D, typename... Dn,
          meta::require<not meta::is_array<D>()>>
Span<T, N>::Span(pointer& data, D d, Dn... dn)
    : data_(data)
    , dim_{ d, dn... }
    , child_scope_(data[0], dn...)
{
}

template <typename T, std::size_t N>
auto Span<T, N>::operator[] (size_type i) noexcept -> reference
{
    child_scope_.data(data_[i]);

    return child_scope_;
}

template <typename T, std::size_t N>
auto Span<T, N>::operator[] (size_type i) const noexcept -> const_reference
{
    child_scope_.data(data_[i]);

    return child_scope_;
}

template <typename T>
Span<T, 1>::Span(pointer& data, Dimension dim)
    : data_(data)
{
    dim_[0] = dim[0];
}

template <typename T>
Span<T, 1>::Span(pointer& data, size_type size)
    : data_(data), dim_{ size }
{
}

} // namespace utility

namespace meta
{

namespace detail
{

template <typename> struct is_span : std::false_type {};
template <typename T, std::size_t N>
struct is_span<sifar::utility::Span<T, N>> : std::true_type {};

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
          typename Span = utility::Span<Type, N>,
          SIFAR_REQUIRE(meta::is_span_set<Pointer, D, Dn...>())>
Span make_span(Pointer& data, D d, Dn... dn)
{
    using size_type = typename Span::size_type;
    return { data, static_cast<size_type>(d), static_cast<size_type>(dn)... };
}

} // namespace utility

namespace detail
{

template <class WriteArchive, typename T,
          SIFAR_REQUIRE(meta::is_write_archive<WriteArchive>()
                        and not meta::is_span<T>())>
void raw_span(WriteArchive& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class WriteArchive, typename T,
          SIFAR_REQUIRE(meta::is_write_archive<WriteArchive>()
                        and meta::is_span<T>())>
void raw_span(WriteArchive& archive, T& array)
{
    using size_type = typename T::size_type;

    for (size_type i = 0; i < array.size(); ++i)
        raw_span(archive, array[i]);
}

template <class ReadArchive, typename T,
          SIFAR_REQUIRE(meta::is_read_archive<ReadArchive>()
                        and not meta::is_span<T>())>
void raw_span(ReadArchive& archive, T& data)
{
    archive & data;
}

// serialization of scoped data with previous dimension initialization
template <class ReadArchive, typename T,
          SIFAR_REQUIRE(meta::is_read_archive<ReadArchive>()
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
          SIFAR_REQUIRE(meta::is_write_archive<WriteArchive>() and
                        meta::is_span_set<T, D, Dn...>())>
void span(WriteArchive& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (pointer == nullptr)
        throw "the write span data must be allocated.";

    archive(dimension, dimension_n...);

    auto span_data = utility::make_span(pointer, dimension, dimension_n...);
    detail::raw_span(archive, span_data);
}

template <class ReadArchive, typename T,
          typename D, typename... Dn,
          SIFAR_REQUIRE(meta::is_read_archive<ReadArchive>() and
                        meta::is_span_set<T, D, Dn...>())>
void span(ReadArchive& archive, T& pointer, D& dimension, Dn&... dimension_n)
{
    if (pointer != nullptr)
        throw "the read span data must be initialized to nullptr.";

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
              SIFAR_REQUIRE(meta::is_archive<Archive>())>
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
          SIFAR_REQUIRE(meta::is_span_set<T, D, Dn...>())>
apply::SpanFunctor<T, D, Dn...> span(T& pointer, D& dimension, Dn&... dimension_n)
{
    return { pointer, dimension, dimension_n... };
}

} // inline namespace common

} // namespace sifar

#endif // SIFAR_SPAN_HPP
