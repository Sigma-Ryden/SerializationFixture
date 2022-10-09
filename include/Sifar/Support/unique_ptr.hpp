#ifndef SIFAR_SUPPORT_UNIQUE_PTR_HPP
#define SIFAR_SUPPORT_UNIQUE_PTR_HPP

#include <type_traits> // true_type, false_type

#include <memory> // unique_ptr

#include <Sifar/WriteArchive.hpp>
#include <Sifar/ReadArchive.hpp>

#include <Sifar/TypeRegistry.hpp>

namespace sifar
{

namespace meta
{

template <typename> struct is_std_unique_ptr : std::false_type {};
template <typename T, typename Deleter>
struct is_std_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

} // namespace meta

namespace library
{

SERIALIZATION_SAVE_DATA(unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    auto data = unique_ptr.get();

    archive & data;

    return archive;
}

SERIALIZATION_LOAD_DATA(unique_ptr, meta::is_std_unique_ptr<T>::value)
{
    using element_type = typename T::element_type;

    element_type* data = nullptr;
    archive & data;

    unique_ptr.reset(data);

    return archive;
}

} // namespace library

} // namespace sifar

SERIALIZATION_CONDITIONAL_TYPE_REGISTRY(meta::is_std_unique_ptr<T>::value)

#endif // SIFAR_SUPPORT_UNIQUE_PTR_HPP
