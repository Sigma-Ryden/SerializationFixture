#include "DerivedObject.hpp"

SERIALIZATION_IMPLEMENTATION(save, self, DerivedObject)
{
    archive << hierarchy<BaseObject>(self) << self.data;
}

SERIALIZATION_IMPLEMENTATION(load, self, DerivedObject)
{
    archive >> hierarchy<BaseObject>(self) >> self.data;
}
