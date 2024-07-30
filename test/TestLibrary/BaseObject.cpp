#include "BaseObject.hpp"

#include <SF/BuiltIn/string.hpp>

SERIALIZATION_IMPLEMENTATION(save, self, BaseObject)
{
    archive << self.id << self.name;
}

SERIALIZATION_IMPLEMENTATION(load, self, BaseObject)
{
    archive >> self.id >> self.name;
}
