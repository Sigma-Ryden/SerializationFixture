#include <SFTestingBase.hpp>

automation::registry_t* sf_testing_base()
{
    static automation::registry_t registry;
    return &registry;
}
