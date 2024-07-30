#ifndef SF_BUILT_IN_VALARRAY_HPP
#define SF_BUILT_IN_VALARRAY_HPP

#include <type_traits> // true_type, false_type

#include <valarray> // valarray

#include <SF/Core/TypeCore.hpp>
#include <SF/Core/Serialization.hpp>

#include <SF/Compress.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_valarray : std::false_type {};
template <typename T> struct is_std_valarray<std::valarray<T>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, valarray, ::sf::meta::is_std_valarray<T>::value)
{
    ::sf::let::u64 size = valarray.size();
    archive & size;

    ::sf::compress::zip(archive, valarray);
}

CONDITIONAL_SERIALIZATION(load, valarray, ::sf::meta::is_std_valarray<T>::value)
{
    ::sf::let::u64 size{};
    archive & size;

    valarray.resize(size);
    ::sf::compress::zip(archive, valarray);
}

#endif // SF_BUILT_IN_VALARRAY_HPP
