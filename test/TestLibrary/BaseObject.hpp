#ifndef BASE_OBJECT_HPP
#define BASE_OBJECT_HPP

#include <string>

#include <SFTestingBase.hpp>

struct BaseObject : sf::instantiable_t
{
    int id;
    std::string name;
};

SERIALIZATION_DECLARATION(save, self, BaseObject)
SERIALIZATION_DECLARATION(load, self, BaseObject)

#endif // BASE_OBJECT_HPP
