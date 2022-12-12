#ifndef SIFAR_CONFIG_HPP
#define SIFAR_CONFIG_HPP

namespace sifar
{

static_assert(sizeof(char) == 1, "Require character size.");

static_assert(sizeof(float) == 4, "Require floating point number size.");
static_assert(sizeof(double) == 8, "Require double precision floation point number size.");

static_assert(sizeof(int) == 4, "Require integer number size.");

} // namespace sifar

#endif // SIFAR_CONFIG_HPP
