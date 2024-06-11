#ifndef SF_BUILT_IN_DEQUE_HPP
#define SF_BUILT_IN_DEQUE_HPP

#include <type_traits> // true_type, false_type

#include <deque> // deque

#include <SF/Core/TypeRegistry.hpp>
#include <SF/Core/TypeCore.hpp>

#include <SF/Compress.hpp>
#include <SF/ExternSerialization.hpp>

namespace sf
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

    compress::slow(archive, deque);

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, deque, meta::is_std_deque<T>::value)
{
    let::u64 size{};
    archive & size;

    deque.resize(size);
    compress::slow(archive, deque);

    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_deque<T>::value)

#endif // SF_BUILT_IN_DEQUE_HPP
