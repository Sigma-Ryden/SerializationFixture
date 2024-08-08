#include "DerivedObject.hpp"

EXPORT_INSTANTIABLE(DerivedObject)

SERIALIZATION_DEFINITION(save, self, DerivedObject)
{
    archive << hierarchy<BaseObject>(self) << self.data;
}

SERIALIZATION_DEFINITION(load, self, DerivedObject)
{
    archive >> hierarchy<BaseObject>(self) >> self.data;
}
