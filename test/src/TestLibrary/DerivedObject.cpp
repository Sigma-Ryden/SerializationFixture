#include "DerivedObject.hpp"

SERIALIZABLE(saveload, self, DerivedObject)
    SERIALIZATION
    (
        archive & hierarchy<BaseObject>(self) & self.data;
    )
SERIALIZABLE_INIT()
