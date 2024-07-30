#ifndef SF_BUILT_IN_FORWARD_LIST_HPP
#define SF_BUILT_IN_FORWARD_LIST_HPP

#include <type_traits> // true_type, false_type

#include <forward_list> // forward_list

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_forward_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_forward_list<std::forward_list<T, Alloc>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, forward_list, ::sf::meta::is_std_forward_list<T>::value)
{
    ::sf::let::u64 size = std::distance(forward_list.begin(), forward_list.end());
    archive & size;

    ::sf::compress::slow(archive, forward_list);
}

CONDITIONAL_SERIALIZATION(load, forward_list, ::sf::meta::is_std_forward_list<T>::value)
{
    ::sf::let::u64 size{};
    archive & size;

    forward_list.resize(size);
    ::sf::compress::slow(archive, forward_list);
}

#endif // SF_BUILT_IN_FORWARD_LIST_HPP
