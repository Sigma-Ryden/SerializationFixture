#ifndef EIGHTSER_DYNAMIC_INSTANTIABLE_HPP
#define EIGHTSER_DYNAMIC_INSTANTIABLE_HPP

// By default library will use instantiable_t type for general instancing,
// if you want to specify behaviour, just define own INSTANTIABLE_TYPE
// Note that: any defined INSTANTIABLE_TYPE must have virtual destructor
#ifndef INSTANTIABLE_TYPE
    #define INSTANTIABLE_TYPE ::eightser::instantiable_t
#endif // INSTANTIABLE_TYPE

namespace eightser
{

struct instantiable_t { virtual ~instantiable_t() = default; };

} // namespace eightser

#endif // EIGHTSER_DYNAMIC_INSTANTIABLE_HPP
