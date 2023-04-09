#ifndef DERIVED_OBJECT_HPP
#define DERIVED_OBJECT_HPP

#include <any>

#include "SFTestingBase.hpp"
#include "BaseObject.hpp"

struct DerivedObject : BaseObject
{
    SERIALIZABLE(DerivedObject)

    std::any data;
};

SERIALIZATION_INTERFACE(SaveLoad, DerivedObject) // will use as load
SERIALIZATION_INTERFACE(Load, DerivedObject)

#endif // DERIVED_OBJECT_HPP
