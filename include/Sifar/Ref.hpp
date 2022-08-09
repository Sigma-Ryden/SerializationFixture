#ifndef SIFAR_REF_HPP
#define SIFAR_REF_HPP

#include <memory> // addressof

#include <Sifar/Detail/Meta.hpp>

namespace sifar
{

namespace utility
{

template <typename T>
class Ref
{
private:
    T* data_;

public:
    using type = T;

    // DONT use dereferencing of null data before rebining
    Ref() : data_(nullptr) {}
    Ref(T& data) : data_(std::addressof(data)) {}

    // rebinding data
    Ref(const Ref&) = default;
    Ref& operator=(const Ref&) = default;

    bool is_null() const noexcept { return data_ == nullptr; }

    operator T&() const noexcept { return get(); }

    T& get() const noexcept { return *data_; }
    void set(T& data) noexcept { data_ = std::addressof(data); }
};

} // namespace utility

namespace meta
{

namespace detail
{

template <typename>
struct is_ref : std::false_type {};

template <typename T>
struct is_ref<sifar::utility::Ref<T>> : std::true_type {};

} // namespcae detail

template <class T> constexpr bool is_ref() noexcept
{
    return detail::is_ref<T>::value;
}

} // namespace meta

} // namespace sifar

#endif // SIFAR_REF_HPP
