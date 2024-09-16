#ifndef SF_UTILITY_FUNCTIONAL_HPP
#define SF_UTILITY_FUNCTIONAL_HPP

#include <cstddef> // size_t

#include <memory> // addressof
#include <valarray> // valarray

#include <SerializationFixture/Detail/Meta.hpp>

namespace sf
{

namespace utility
{

template <class ContainerType>
typename ContainerType::value_type const* data(ContainerType const& c) noexcept
{
    // if begin return iterator - we should deref it and take address again
    return std::addressof(*std::begin(c));
}

template <typename ValueType>
ValueType const* data(std::valarray<ValueType> const& c) noexcept
{
    return std::begin(c);
}

template <typename ValueType, std::size_t SizeValue>
ValueType const* data(ValueType const (&array)[SizeValue]) noexcept
{
    return array;
}

template <class ContainerType>
auto size(ContainerType const& c) noexcept -> decltype(c.size())
{
    return c.size();
}

template <typename ValueType, std::size_t SizeValue>
constexpr std::size_t size(ValueType const (&)[SizeValue]) noexcept
{
    return SizeValue;
}

} // namespace utility

} // namespace sf

#endif // SF_UTILITY_FUNCTIONAL_HPP
