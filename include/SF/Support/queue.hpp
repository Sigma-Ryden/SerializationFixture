#ifndef SF_SUPPORT_QUEUE_HPP
#define SF_SUPPORT_QUEUE_HPP

#include <type_traits> // true_type, false_type

#include <queue> // queue

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

#include <SF/Support/Detail/Meta.hpp>

// default container for queue
#include <SF/Support/deque.hpp>

// default container for priority_queue
#include <SF/Support/vector.hpp>

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

template <class T> constexpr bool is_std_any_queue() noexcept
{
    return is_std_queue<T>::value
        or is_std_priority_queue<T>::value;
}

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, queue, meta::is_std_any_queue<T>())
{
    archive & meta::underlying(queue);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_any_queue<T>())

#endif // SF_SUPPORT_QUEUE_HPP
