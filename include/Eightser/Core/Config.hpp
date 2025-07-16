#ifndef EIGHTSER_CORE_CONFIG_HPP
#define EIGHTSER_CORE_CONFIG_HPP

namespace eightser
{

#ifndef EIGHTSER_TYPE_PORTABLE_DISABLE

static_assert(sizeof(char) == 1, "Require character size.");

static_assert(sizeof(float) == 4, "Require floating point number size.");
static_assert(sizeof(double) == 8, "Require double precision floation point number size.");

static_assert(sizeof(int) == 4, "Require integer number size.");

#endif // EIGHTSER_TYPE_PORTABLE_DISABLE

} // namespace eightser

#endif // EIGHTSER_CORE_CONFIG_HPP
