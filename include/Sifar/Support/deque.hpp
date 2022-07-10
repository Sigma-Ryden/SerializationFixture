#ifndef SIFAR_SUPPORT_DEQUE_HPP
#define SIFAR_SUPPORT_DEQUE_HPP

#include <type_traits> // true_type, false_type

#include <deque> // deque

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_deque : std::false_type {};
template <typename T, typename Alloc>
struct is_std_deque<std::deque<T, Alloc>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(deque, meta::is_std_deque<T>::value)
{
    let::u64 size = deque.size();
    archive & size;

    for (const auto& item : deque)
        archive & item;

    return archive;
}

SERIALIZATION_LOAD_DATA(deque, meta::is_std_deque<T>::value)
{
    let::u64 size;
    archive & size;

    deque.resize(size);

    for (auto& item : deque)
        archive & item;

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_TYPE_REGISTRY_IF(meta::is_std_deque<T>::value)

#endif // SIFAR_SUPPORT_DEQUE_HPP
