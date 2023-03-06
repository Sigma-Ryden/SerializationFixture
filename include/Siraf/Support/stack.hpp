#ifndef SIRAF_SUPPORT_STACK_HPP
#define SIRAF_SUPPORT_STACK_HPP

#include <type_traits> // true_type, false_type

#include <stack> // stack

#include <Siraf/Core/TypeRegistry.hpp>
#include <Siraf/ExternSerialization.hpp>

#include <Siraf/Support/Detail/Meta.hpp>

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

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(SaveLoad, stack, meta::is_std_stack<T>::value)
{
    archive & meta::underlying(stack);
    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_stack<T>::value)

#endif // SIRAF_SUPPORT_STACK_HPP
