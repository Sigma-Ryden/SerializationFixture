#include "BaseObject.hpp"

#include <SF/BuiltIn/string.hpp>

SERIALIZATION_DEFINITION(save, self, BaseObject)
{
    archive << self.id << self.name;
}

SERIALIZATION_DEFINITION(load, self, BaseObject)
{
    archive >> self.id >> self.name;
}
