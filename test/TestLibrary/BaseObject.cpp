#include "BaseObject.hpp"

#include <SerializationFixture/BuiltIn/string.hpp>

SERIALIZABLE(save, self, BaseObject)
    SERIALIZATION
    (
        archive << self.id << self.name;
    )
SERIALIZABLE_INIT()

SERIALIZABLE(load, self, BaseObject)
    SERIALIZATION
    (
        archive >> self.id >> self.name;
    )
SERIALIZABLE_INIT()
