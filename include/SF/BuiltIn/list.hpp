#ifndef SF_BUILT_IN_LIST_HPP
#define SF_BUILT_IN_LIST_HPP

#include <type_traits> // true_type, false_type

#include <list> // list

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_list : std::false_type {};
template <typename T, typename Alloc>
struct is_std_list<std::list<T, Alloc>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, list, ::sf::meta::is_std_list<T>::value)
{
    ::sf::let::u64 size = list.size();
    archive & size;

    ::sf::compress::slow(archive, list);
}

CONDITIONAL_SERIALIZATION(load, list, ::sf::meta::is_std_list<T>::value)
{
    ::sf::let::u64 size{};
    archive & size;

    list.resize(size);
    ::sf::compress::slow(archive, list);
}

#endif // SF_BUILT_IN_LIST_HPP
