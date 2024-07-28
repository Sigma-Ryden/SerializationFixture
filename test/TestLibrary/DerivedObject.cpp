#include "DerivedObject.hpp"

SERIALIZATION_IMPLEMENTATION(saveload, DerivedObject)
{
    archive << hierarchy<BaseObject>(self) << self.data;
}

SERIALIZATION_IMPLEMENTATION(load, DerivedObject)
{
    archive >> hierarchy<BaseObject>(self) >> self.data;
}
