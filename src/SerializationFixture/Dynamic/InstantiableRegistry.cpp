#include <SerializationFixture/Dynamic/InstantiableRegistry.hpp>

namespace sf
{

namespace dynamic
{

template struct instantiable_registry_t<INSTANTIABLE_VOID_POINTER_TYPES>;
instantiable_registry_t<INSTANTIABLE_VOID_POINTER_TYPES> instantiable_registry;

} // namespace dynamic

} // namespace sf
