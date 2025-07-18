#include <Eightser/Dynamic/InstantiableRegistry.hpp>

namespace eightser
{

namespace dynamic
{

instantiable_registry_t* instantiable_registry()
{
    static instantiable_registry_t self; return &self;
}

} // namespace dynamic

} // namespace eightser
