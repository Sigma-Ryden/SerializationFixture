#ifndef BASE_OBJECT_HPP
#define BASE_OBJECT_HPP

#include <string>

#include <SFTestingBase.hpp>

struct BaseObject : sf::instantiable_t
{
    // not required for SERIALIZATION_INTERFACE
    SERIALIZABLE(BaseObject)

    int id;
    std::string name;
};

SERIALIZATION_INTERFACE(save, self, BaseObject)
SERIALIZATION_INTERFACE(load, self, BaseObject)

#endif // BASE_OBJECT_HPP
