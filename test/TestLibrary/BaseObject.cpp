#include "BaseObject.hpp"

#include <Eightser/BuiltIn/string.hpp>

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
