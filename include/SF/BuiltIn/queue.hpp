#ifndef SF_BUILT_IN_QUEUE_HPP
#define SF_BUILT_IN_QUEUE_HPP

#include <type_traits> // true_type, false_type

#include <queue> // queue

#include <SF/Core/Serialization.hpp>

#include <SF/BuiltIn/Detail/Meta.hpp>

// default container for queue
#include <SF/BuiltIn/deque.hpp>

// default container for priority_queue
#include <SF/BuiltIn/vector.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_queue : std::false_type {};
template <typename T, class Container>
struct is_std_queue<std::queue<T, Container>> : std::true_type {};

template <typename> struct is_std_priority_queue : std::false_type {};
template <typename T, class Container, class Compare>
struct is_std_priority_queue<std::priority_queue<T, Container, Compare>> : std::true_type {};

template <class T> struct is_std_any_queue
    : one<is_std_queue<T>,
          is_std_priority_queue<T>> {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, queue, ::sf::meta::is_std_any_queue<T>::value)
{
    archive & ::sf::meta::underlying(queue);
}

#endif // SF_BUILT_IN_QUEUE_HPP
