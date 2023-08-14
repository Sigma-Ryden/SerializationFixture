#ifndef SF_DYNAMIC_INSTANTIABLE_HPP
#define SF_DYNAMIC_INSTANTIABLE_HPP

#include <SF/Core/Serialization.hpp>
#include <SF/Core/InstantiableTrait.hpp>
#include <SF/Core/Hash.hpp>

// By default library will use Instantiable type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::sf::Instantiable
#endif // INSTANTIABLE_TYPE

namespace sf
{

// Should be in sf namespace
struct Instantiable { virtual ~Instantiable() = default; };

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_HPP
