#include "DerivedObject.hpp"

EXPORT_INSTANTIABLE(DerivedObject)

SERIALIZABLE(saveload, self, DerivedObject)
    SERIALIZATION
    (
        archive & hierarchy<BaseObject>(self) & self.data;
    )
SERIALIZABLE_INIT()
