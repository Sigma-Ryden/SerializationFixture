#include "DerivedObject.hpp"

SERIALIZATION_DEFINITION(save, self, DerivedObject)
{
    archive << hierarchy<BaseObject>(self) << self.data;
}

SERIALIZATION_DEFINITION(load, self, DerivedObject)
{
    archive >> hierarchy<BaseObject>(self) >> self.data;
}
