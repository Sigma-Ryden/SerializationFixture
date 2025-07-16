#ifndef DERIVED_OBJECT_HPP
#define DERIVED_OBJECT_HPP

#include <EightserTestingBase.hpp>
#include "BaseObject.hpp"

struct DerivedObject : BaseObject
{
    float data;
};

SERIALIZABLE_DECLARATION(DerivedObject)
    INSTANTIABLE(S)
SERIALIZABLE_DECLARATION_INIT()

#endif // DERIVED_OBJECT_HPP
