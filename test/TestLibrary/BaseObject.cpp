#include "BaseObject.hpp"

#include <SerializationFixture/BuiltIn/string.hpp>

EXPORT_INSTANTIABLE(BaseObject)

SERIALIZATION_DEFINITION(save, self, BaseObject)
{
    archive << self.id << self.name;
}

SERIALIZATION_DEFINITION(load, self, BaseObject)
{
    archive >> self.id >> self.name;
}
