#ifndef SF_SUPPORT_COMPLEX_HPP
#define SF_SUPPORT_COMPLEX_HPP

#include <type_traits> // true_type, false_type

#include <complex> // complex

#include <SF/Core/TypeRegistry.hpp>
#include <SF/ExternSerialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_complex : std::false_type {};
template <typename T>
struct is_std_complex<std::complex<T>> : std::true_type {};

} // namespace meta

inline namespace library
{

EXTERN_CONDITIONAL_SERIALIZATION(Save, complex, meta::is_std_complex<T>::value)
{
    auto real = complex.real();
    auto imag = complex.imag();

    archive & real & imag;

    return archive;
}

EXTERN_CONDITIONAL_SERIALIZATION(Load, complex, meta::is_std_complex<T>::value)
{
    using integral_type = typename T::value_type;

    integral_type real{};
    integral_type imag{};

    archive & real & imag;

    complex = {real, imag};
    return archive;
}

} // inline namespace library

} // namespace sf

CONDITIONAL_TYPE_REGISTRY(meta::is_std_complex<T>::value)

#endif // SF_SUPPORT_COMPLEX_HPP
