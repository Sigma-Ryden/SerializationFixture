#ifndef SF_DYNAMIC_INSTANTIABLE_HPP
#define SF_DYNAMIC_INSTANTIABLE_HPP

// By default library will use instantiable_t type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
// Note that: any defined INSTANTIABLE_TYPE must have virtual destructor
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::sf::instantiable_t
#endif // INSTANTIABLE_TYPE

namespace sf
{

struct instantiable_t { virtual ~instantiable_t() = default; };

} // namespace sf

#endif // SF_DYNAMIC_INSTANTIABLE_HPP
