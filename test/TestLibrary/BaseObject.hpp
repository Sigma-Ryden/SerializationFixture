#ifndef BASE_OBJECT_HPP
#define BASE_OBJECT_HPP

#include <string>

#include <EightserTestingBase.hpp>

struct BaseObject : eightser::instantiable_t
{
    int id;
    std::string name;
};

SERIALIZABLE_DECLARATION(BaseObject)
    INSTANTIABLE(S)
SERIALIZABLE_DECLARATION_INIT()

#endif // BASE_OBJECT_HPP
