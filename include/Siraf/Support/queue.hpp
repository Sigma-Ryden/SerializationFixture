#ifndef SIRAF_SUPPORT_QUEUE_HPP
#define SIRAF_SUPPORT_QUEUE_HPP

#include <type_traits> // true_type, false_type

#include <queue> // queue

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

// default container for queue
#include <Siraf/Support/deque.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_queue : std::false_type {};
template <typename T, class Container>
struct is_std_queue<std::queue<T, Container>> : std::true_type {};

} // namespace meta

namespace detail
{

template <typename Type, class Container,
          class Base = std::queue<Type, Container>>
Container& underlying(std::queue<Type, Container>& queue)
{
    struct queue_inner : public Base
    {
        static Container& underlying(Base& base)
        {
            auto underlying_container = &queue_inner::c;
            return base.*underlying_container;
        }
    };

    return queue_inner::underlying(queue);
}

} // namespace detail

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, queue, meta::is_std_queue<T>::value)
{
    archive & detail::underlying(queue);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_queue<T>::value)

#endif // SIRAF_SUPPORT_QUEUE_HPP
