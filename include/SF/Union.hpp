#ifndef SF_UNION_HPP
#define SF_UNION_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Binary.hpp>

namespace sf
{

namespace meta
{

template <typename T> struct is_serializable_union
    : all<std::is_union<T>,
          negation<meta::is_has_any_save_mode<T>>,
          negation<meta::is_has_any_save_mode<T>>> {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(saveload, data, ::sf::meta::is_serializable_union<T>::value)
{
    ::sf::binary(archive, data);
}

#endif // SF_UNION_HPP
