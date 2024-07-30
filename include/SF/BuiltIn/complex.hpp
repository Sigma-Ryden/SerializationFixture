#ifndef SF_BUILT_IN_COMPLEX_HPP
#define SF_BUILT_IN_COMPLEX_HPP

#include <type_traits> // true_type, false_type

#include <complex> // complex

#include <SF/Core/Serialization.hpp>

namespace sf
{

namespace meta
{

template <typename> struct is_std_complex : std::false_type {};
template <typename T> struct is_std_complex<std::complex<T>> : std::true_type {};

} // namespace meta

} // namespace sf

CONDITIONAL_SERIALIZATION(save, complex, ::sf::meta::is_std_complex<T>::value)
{
    auto re = complex.real();
    auto im = complex.imag();

    archive & re & im;
}

CONDITIONAL_SERIALIZATION(load, complex, ::sf::meta::is_std_complex<T>::value)
{
    using integral_type = typename T::value_type;

    integral_type re{};
    integral_type im{};

    archive & re & im;

    complex.real(re);
    complex.imag(im);
}

#endif // SF_BUILT_IN_COMPLEX_HPP
