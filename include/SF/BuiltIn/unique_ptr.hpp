#ifndef SF_BUILT_IN_UNIQUE_PTR_HPP
#define SF_BUILT_IN_UNIQUE_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // unique_ptr

#include <SF/Core/Memory.hpp>
#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_unique_ptr : std::false_type {};
template <typename T, typename Deleter>
struct is_std_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, unique_ptr, ::sf::meta::is_std_unique_ptr<T>::value)
{
    auto data = unique_ptr.get();
    archive & data;
}

CONDITIONAL_SERIALIZATION(load, unique_ptr, ::sf::meta::is_std_unique_ptr<T>::value)
{
    using item_type = typename ::sf::memory::ptr_traits<T>::item;

    item_type* data = nullptr;
    archive & data;

    unique_ptr.reset(data);
}

#endif // SF_BUILT_IN_UNIQUE_PTR_HPP
