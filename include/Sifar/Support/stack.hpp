#ifndef SIFAR_SUPPORT_STACK_HPP
#define SIFAR_SUPPORT_STACK_HPP

#include <type_traits> // true_type, false_type

#include <stack> // stack

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>
#include <Sifar/UnifiedData.hpp>

#include <Sifar/TypeRegistry.hpp>

// default container for stack
#include <Sifar/Support/deque.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_stack : std::false_type {};
template <typename T, class Container>
struct is_std_stack<std::stack<T, Container>> : std::true_type {};

} // namespace meta

namespace detail
{

template <typename Type, class Container,
          class Base = std::stack<Type, Container>>
Container& underlying(std::stack<Type, Container>& stack)
{
    struct stack_inner : public Base
    {
        static Container& underlying(Base& base)
        {
            auto underlying_container = &stack_inner::c;
            return base.*underlying_container;
        }
    };

    return stack_inner::underlying(stack);
}

} // namespace detail

inline namespace library
{

SERIALIZATION_SAVE_LOAD_DATA(stack, meta::is_std_stack<T>::value)
{
    archive & detail::underlying(stack);

    return archive;
}

} // inline namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_stack<T>::value)

#endif // SIFAR_SUPPORT_STACK_HPP
