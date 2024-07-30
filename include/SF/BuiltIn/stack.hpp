#ifndef SF_BUILT_IN_STACK_HPP
#define SF_BUILT_IN_STACK_HPP

#include <type_traits> // true_type, false_type

#include <stack> // stack

#include <SF/Core/Serialization.hpp>

#include <SF/BuiltIn/Detail/Meta.hpp>

// default container for stack
#include <SF/BuiltIn/deque.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_stack : std::false_type {};
template <typename T, class Container>
struct is_std_stack<std::stack<T, Container>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, stack, ::sf::meta::is_std_stack<T>::value)
{
    archive & ::sf::meta::underlying(stack);
}

#endif // SF_BUILT_IN_STACK_HPP
