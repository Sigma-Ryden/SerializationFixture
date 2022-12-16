#ifndef SIRAF_SUPPORT_STACK_HPP
#define SIRAF_SUPPORT_STACK_HPP

#include <type_traits> // true_type, false_type

#include <stack> // stack

#include <Siraf/WriteArchive.hpp>
#include <Siraf/ReadArchive.hpp>
#include <Siraf/UnifiedData.hpp>

#include <Siraf/TypeRegistry.hpp>

// default container for stack
#include <Siraf/Support/deque.hpp>

namespace siraf
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

CONDITIONAL_SAVE_LOAD_SERIALIZABLE_TYPE(stack, meta::is_std_stack<T>::value)
{
    archive & detail::underlying(stack);

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_REGISTRY_SERIALIZABLE_TYPE(meta::is_std_stack<T>::value)

#endif // SIRAF_SUPPORT_STACK_HPP
