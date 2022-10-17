#ifndef SIFAR_SUPPORT_QUEUE_HPP
#define SIFAR_SUPPORT_QUEUE_HPP

#include <type_traits> // true_type, false_type

#include <queue> // queue

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/UnifiedData.hpp>

#include <Sifar/TypeRegistry.hpp>

// default container for queue
#include <Sifar/Support/deque.hpp>

namespace sifar
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

SERIALIZATION_SAVE_LOAD_DATA(queue, meta::is_std_queue<T>::value)
{
    archive & detail::underlying(queue);

    return archive;
}

} // inline namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_queue<T>::value)

#endif // SIFAR_SUPPORT_QUEUE_HPP
