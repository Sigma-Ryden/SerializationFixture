#ifndef BASE_OBJECT_HPP
#define BASE_OBJECT_HPP

#include <string>

#include <SFTestingBase.hpp>

struct BaseObject : sf::instantiable_t
{
    int id;
    std::string name;
};

SERIALIZABLE_DECLARATION(BaseObject)
SERIALIZABLE_DECLARATION_INIT()

#endif // BASE_OBJECT_HPP
