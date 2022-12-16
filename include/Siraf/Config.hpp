#ifndef SIRAF_CONFIG_HPP
#define SIRAF_CONFIG_HPP

namespace siraf
{

static_assert(sizeof(char) == 1, "Require character size.");

static_assert(sizeof(float) == 4, "Require floating point number size.");
static_assert(sizeof(double) == 8, "Require double precision floation point number size.");

static_assert(sizeof(int) == 4, "Require integer number size.");

} // namespace siraf

#endif // SIRAF_CONFIG_HPP
