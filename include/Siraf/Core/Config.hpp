#ifndef SIRAF_CORE_CONFIG_HPP
#define SIRAF_CORE_CONFIG_HPP

#if __cplusplus < 201703L
    #define _SIRAF_STATIC_INLINE
#else
    #define _SIRAF_STATIC_INLINE static inline
#endif

namespace siraf
{

namespace core
{

#ifndef SIRAF_DEFAULT_DISABLE

static_assert(sizeof(char) == 1, "Require character size.");

static_assert(sizeof(float) == 4, "Require floating point number size.");
static_assert(sizeof(double) == 8, "Require double precision floation point number size.");

static_assert(sizeof(int) == 4, "Require integer number size.");

#endif // SIRAF_DEFAULT_DISABLE

} // namespace core

} // namespace siraf

#endif // SIRAF_CORE_CONFIG_HPP
