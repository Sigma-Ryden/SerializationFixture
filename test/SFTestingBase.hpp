#ifndef SF_TESTING_BASE_HPP
#define SF_TESTING_BASE_HPP

#include <SF/Core.hpp>

#define AUTOMATION_REGISTRY sf_testing_base()
#include <Automation.hpp>

extern automation::registry_t* sf_testing_base();

using sf::oarchive;
using sf::iarchive;

using sf::hierarchy;

using sf::instantiable_t;

#endif // SF_TESTING_BASE_HPP
