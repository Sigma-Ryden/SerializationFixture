#ifndef SERIALIZATION_SCOPE_HPP
#define SERIALIZATION_SCOPE_HPP

#include <cstddef> // size_t
#include <initializer_list> // initializer_list

#include <Serialization/Ref.hpp>

#include <Serialization/Detail/Pointer.hpp>
#include <Serialization/Detail/Tools.hpp>

#include <Serialization/Detail/Meta.hpp>

namespace serialization
{

namespace utility
{

template <typename T, std::size_t N>
class Scope
{
public:
    using size_type         = std::size_t;
    using value_type        = Scope<T, N - 1>;
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
    Scope(pointer& data, Dimension dim_);

    template <typename D, typename... Dn,
              meta::require<not meta::is_array<D>()> = 0>
    Scope(pointer& data, D d, Dn... dn);

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
class Scope<T, 1>
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
    Scope(pointer& data, Dimension size);
    Scope(pointer& data, size_type size);

    void init(pointer data) noexcept { data_.get() = data; }
    void data(Ref<pointer> data) noexcept { data_ = data; }

    pointer& data() noexcept { return data_.get(); }
    const_pointer& data() const noexcept { return data_.get(); }

    size_type size() const noexcept { return dim_[0]; }

    Dimension& dim() noexcept { return dim_; }
    const Dimension& dim() const noexcept { return dim_; }

    reference operator[] (size_type i) noexcept { return data_[i]; }
    const_reference operator[] (size_type i) const noexcept { return data_[i]; }
};

template <typename T, std::size_t N>
Scope<T, N>::Scope(pointer& data, Dimension dim)
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
Scope<T, N>::Scope(pointer& data, D d, Dn... dn)
    : data_(data)
    , dim_{ d, dn... }
    , child_scope_(data[0], dn...)
{
}

template <typename T, std::size_t N>
auto Scope<T, N>::operator[] (size_type i) noexcept -> reference
{
    child_scope_.data(data_[i]);

    return child_scope_;
}

template <typename T, std::size_t N>
auto Scope<T, N>::operator[] (size_type i) const noexcept -> const_reference
{
    child_scope_.data(data_[i]);

    return child_scope_;
}

template <typename T>
Scope<T, 1>::Scope(pointer& data, Dimension dim)
    : data_(data)
{
    dim_[0] = dim[0];
}

template <typename T>
Scope<T, 1>::Scope(pointer& data, size_type size)
    : data_(data), dim_{ size }
{
}

} // namespace utility

template <typename Pointer, typename D, typename... Dn,
         std::size_t N = sizeof...(Dn) + 1,
         typename Type = meta::remove_pointer<Pointer, N>,
         typename Scope = utility::Scope<Type, N>,
         meta::require<meta::and_<std::is_arithmetic<D>,
                                  std::is_arithmetic<Dn>...>::value> = 0>
Scope zip(Pointer& data, D d, Dn... dn)
{
    using size_type = typename Scope::size_type;

    return Scope(data, static_cast<size_type>(d), static_cast<size_type>(dn)...);
}

namespace meta
{

namespace detail
{

template <typename>
struct is_scope : std::false_type {};

template <typename T, std::size_t N>
struct is_scope<serialization::utility::Scope<T, N>> : std::true_type {};

} // namespcae detail

template <class T> constexpr bool is_scope() noexcept
{
    return detail::is_scope<T>::value;
}

} // namespace meta

} // namespace serialization

#endif // SERIALIZATION_SCOPE_HPP
