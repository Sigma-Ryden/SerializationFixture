#ifndef SF_BUILT_IN_WEAK_PTR_HPP
#define SF_BUILT_IN_WEAK_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // weak_ptr

#include <SF/Core/Memory.hpp>
#include <SF/Core/Serialization.hpp>

// serialization of shared_ptr
#include <SF/BuiltIn/shared_ptr.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_weak_ptr : std::false_type {};
template <typename T> struct is_std_weak_ptr<std::weak_ptr<T>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, weak_ptr, ::sf::meta::is_std_weak_ptr<T>::value)
{
    auto sptr = weak_ptr.lock();
    archive & sptr;
}

CONDITIONAL_SERIALIZATION(load, weak_ptr, ::sf::meta::is_std_weak_ptr<T>::value)
{
    using item_type = typename ::sf::memory::ptr_traits<T>::item;

    std::shared_ptr<item_type> sptr;
    archive & sptr;

    weak_ptr = sptr;
}

#endif // SF_BUILT_IN_WEAK_PTR_HPP
