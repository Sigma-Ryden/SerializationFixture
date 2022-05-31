#ifndef SERIALIZATION_SCOPE_HPP
#define SERIALIZATION_SCOPE_HPP

#include <cstddef> // size_t
#include <initializer_list> // initializer_list

#include <Serialization/Detail/Pointer.hpp>
#include <Serialization/Detail/Tools.hpp>

#include <Serialization/Detail/Meta.hpp>

namespace serialization
{

namespace utility
{

template <std::size_t N>
class Dimention
{
public:
    using size_type         = std::size_t;
    using value_type        = std::size_t;

    using reference         = value_type&;
    using const_reference   = const value_type&;

    using pointer           = value_type*;
    using const_pointer     = const value_type*;

private:
    value_type data_[N];

public:
    Dimention(std::initializer_list<value_type> list) noexcept;
    Dimention(const_pointer first, const_pointer last) noexcept;

    reference operator[](size_type i) noexcept;
    const_reference operator[](size_type i) const noexcept;

    pointer data() noexcept;
    const_pointer data() const noexcept;

    size_type size() const noexcept;
};

template <>
class Dimention<1>
{
public:
    using size_type         = std::size_t;
    using value_type        = std::size_t;

    using pointer           = value_type*;
    using const_pointer     = const value_type*;

private:
    value_type data_;

public:
    // DON'T mark this constructor as explicit, for variadic 'dim' function
    // is the same as for initializer_list with single arg
    Dimention(value_type value) noexcept;
    Dimention(const_pointer first, const_pointer /*last*/) noexcept;

    operator size_type() const noexcept;

    size_type size() const noexcept;
};

template <std::size_t N>
Dimention<N>::Dimention(std::initializer_list<value_type> list) noexcept
{
    utility::copy(list.begin(), list.end(), data_);
}

template <std::size_t N>
Dimention<N>::Dimention(const_pointer first, const_pointer last) noexcept
{
    utility::copy(first, last, data_);
}

template <std::size_t N>
auto Dimention<N>::operator[](size_type i) noexcept -> reference
{
    return data_[i];
}

template <std::size_t N>
auto Dimention<N>::operator[](size_type i) const noexcept -> const_reference
{
    return data_[i];
}

template <std::size_t N>
auto Dimention<N>::data() noexcept -> pointer
{
    return data_;
}

template <std::size_t N>
auto Dimention<N>::data() const noexcept -> const_pointer
{
    return data_;
}

template <std::size_t N>
auto Dimention<N>::size() const noexcept -> size_type
{
    return N;
}

Dimention<1>::Dimention(const_pointer first, const_pointer /*last*/) noexcept
{
    data_ = *first;
}

Dimention<1>::Dimention(value_type value) noexcept
{
    data_ = value;
}

Dimention<1>::operator size_type() const noexcept
{
    return data_;
}

auto Dimention<1>::size() const noexcept -> size_type
{
    return 1;
}

template <typename T, std::size_t N>
class Scope
{
public:
    using size_type         = std::size_t;
    using value_type        = Scope<T, N - 1>;

    using reference         = value_type&;
    using const_reference   = const value_type&;

    using pointer           = meta::pointer<T, N>;
    using const_pointer     = const meta::pointer<T, N>;

    using dimention         = Dimention<N>;
    using dimention_next    = Dimention<N - 1>;

private:
    pointer data_;

    size_type size_;
    mutable value_type child_scope_;

public:
    Scope(pointer data, dimention dim);

    void data(pointer data) noexcept;

    pointer data() noexcept;
    const_pointer data() const noexcept;

    size_type size() const noexcept;

    reference operator[] (size_type i) noexcept;
    const_reference operator[] (size_type i) const noexcept;
};

template <typename T>
class Scope<T, 1>
{
public:
    using size_type         = std::size_t;
    using value_type        = T;

    using reference         = T&;
    using const_reference   = const T&;

    using pointer           = meta::pointer<T, 1>;
    using const_pointer     = const meta::pointer<T, 1>;

private:
    pointer data_;

    size_type size_;

public:
    Scope(pointer data, size_type size);

    void data(pointer data) noexcept;

    pointer data() noexcept;
    const_pointer data() const noexcept;

    size_type size() const noexcept;

    reference operator[] (size_type i) noexcept;
    const_reference operator[] (size_type i) const noexcept;
};

template <typename T, std::size_t N>
Scope<T, N>::Scope(pointer data, dimention dim)
    : data_(data)
    , size_(dim[0])
    , child_scope_(nullptr, dimention_next(dim.data() + 1, dim.data() + dim.size()))
{
}

template <typename T, std::size_t N>
void Scope<T, N>::data(pointer data) noexcept
{
    data_ = data;
}

template <typename T, std::size_t N>
auto Scope<T, N>::data() noexcept -> pointer
{
    return data_;
}

template <typename T, std::size_t N>
auto Scope<T, N>::data() const noexcept -> const_pointer
{
    return data_;
}

template <typename T, std::size_t N>
auto Scope<T, N>::size() const noexcept -> size_type
{
    return size_;
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
Scope<T, 1>::Scope(pointer data, size_type size)
    : data_(data), size_(size)
{
}

template <typename T>
void Scope<T, 1>::data(pointer data) noexcept
{
    data_ = data;
}

template <typename T>
auto Scope<T, 1>::data() noexcept -> pointer
{
    return data_;
}

template <typename T>
auto Scope<T, 1>::data() const noexcept -> const_pointer
{
    return data_;
}

template <typename T>
auto Scope<T, 1>::size() const noexcept -> size_type
{
    return size_;
}

template <typename T>
auto Scope<T, 1>::operator[] (size_type i) noexcept -> reference
{
    return data_[i];
}

template <typename T>
auto Scope<T, 1>::operator[] (size_type i) const noexcept -> const_reference
{
    return data_[i];
}

} // namespace utility

template <typename D, typename... Dn,
    meta::require<meta::and_<std::is_arithmetic<D>, std::is_arithmetic<Dn>...>::value> = 0>
auto dim(D d, Dn... dn) -> utility::Dimention<sizeof...(Dn) + 1>
{
    using value_type = typename utility::Dimention<sizeof...(Dn) + 1>::value_type;

    return { static_cast<value_type>(d), static_cast<value_type>(dn)... };
}

template <typename Pointer, std::size_t N, typename T = meta::remove_pointer<Pointer, N>>
utility::Scope<T, N> scope(Pointer data, utility::Dimention<N> dimention)
{
    return { data, dimention };
}

template <typename Pointer, typename T = meta::remove_pointer<Pointer, 1>>
utility::Scope<T, 1> scope(Pointer data, std::size_t dimention)
{
    return { data, dimention };
}

template <typename CharType, meta::require<meta::is_character<CharType>()> = 0>
utility::Scope<CharType, 1> scope(CharType* data)
{
    return { data, utility::size(data) };
}

namespace meta
{

namespace detail
{

template <typename>
struct is_scope : std::false_type {};

template <typename Type, std::size_t Dimention>
struct is_scope<serialization::utility::Scope<Type, Dimention>> : std::true_type {};

} // namespcae detail

template <class T> constexpr bool is_scope() noexcept
{
    return detail::is_scope<T>::value;
}

} // namespace meta

} // namespace serialization

#endif // SERIALIZATION_SCOPE_HPP
