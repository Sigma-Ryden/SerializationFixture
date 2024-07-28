#include "BaseObject.hpp"

#include <SF/BuiltIn/string.hpp>

SERIALIZATION_IMPLEMENTATION(save, BaseObject)
{
    archive << self.id << self.name;
}

SERIALIZATION_IMPLEMENTATION(load, BaseObject)
{
    archive >> self.id >> self.name;
}
