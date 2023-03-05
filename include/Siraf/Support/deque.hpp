#ifndef SIRAF_SUPPORT_DEQUE_HPP
#define SIRAF_SUPPORT_DEQUE_HPP

#include <type_traits> // true_type, false_type

#include <deque> // deque

#include <Siraf/ExternSerialization.hpp>
#include <Siraf/TypeRegistry.hpp>

#include <Siraf/Utility.hpp>

namespace siraf
{

namespace meta
{

template <typename> struct is_std_deque : std::false_type {};
template <typename T, typename Alloc>
struct is_std_deque<std::deque<T, Alloc>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, deque, meta::is_std_deque<T>::value)
{
    let::u64 size = deque.size();
    archive & size;

    for (auto& item : deque)
        archive & item;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, deque, meta::is_std_deque<T>::value)
{
    let::u64 size{};
    archive & size;

    deque.resize(size);

    for (auto& item : deque)
        archive & item;

    return archive;
}

} // inline namespace library

} // namespace siraf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_deque<T>::value)

#endif // SIRAF_SUPPORT_DEQUE_HPP
