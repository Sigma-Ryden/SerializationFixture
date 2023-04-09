#ifndef SF_SUPPORT_STACK_HPP
#define SF_SUPPORT_STACK_HPP

#include <type_traits> // true_type, false_type

#include <stack> // stack

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

#include <SF/Support/Detail/Meta.hpp>

// default container for stack
#include <SF/Support/deque.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_stack : std::false_type {};
template <typename T, class Container>
struct is_std_stack<std::stack<T, Container>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, stack, meta::is_std_stack<T>::value)
{
    archive & meta::underlying(stack);
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_stack<T>::value)

#endif // SF_SUPPORT_STACK_HPP
