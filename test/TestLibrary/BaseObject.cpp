#include "BaseObject.hpp"

#include <SF/BuiltIn/string.hpp>

SERIALIZATION_IMPLEMENTATION(Save, BaseObject)
{
    archive << self.id << self.name;
}

SERIALIZATION_IMPLEMENTATION(Load, BaseObject)
{
    archive >> self.id >> self.name;
}
