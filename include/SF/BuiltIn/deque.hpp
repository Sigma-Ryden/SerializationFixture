#ifndef SF_BUILT_IN_DEQUE_HPP
#define SF_BUILT_IN_DEQUE_HPP

#include <type_traits> // true_type, false_type

#include <deque> // deque

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_deque : std::false_type {};
template <typename T, typename Alloc>
struct is_std_deque<std::deque<T, Alloc>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, deque, ::sf::meta::is_std_deque<T>::value)
{
    ::sf::let::u64 size = deque.size();
    archive & size;

    ::sf::compress::slow(archive, deque);
}

CONDITIONAL_SERIALIZATION(load, deque, ::sf::meta::is_std_deque<T>::value)
{
    ::sf::let::u64 size{};
    archive & size;

    deque.resize(size);
    ::sf::compress::slow(archive, deque);
}

#endif // SF_BUILT_IN_DEQUE_HPP
