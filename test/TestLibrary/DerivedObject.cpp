#include "DerivedObject.hpp"

#include <SF/Support/any.hpp>

SERIALIZATION_IMPLEMENTATION(SaveLoad, DerivedObject)
{
    archive << hierarchy<BaseObject>(self) << self.data;
}

SERIALIZATION_IMPLEMENTATION(Load, DerivedObject)
{
    archive >> hierarchy<BaseObject>(self) >> self.data;
}
