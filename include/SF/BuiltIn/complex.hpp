#ifndef SF_BUILT_IN_COMPLEX_HPP
#define SF_BUILT_IN_COMPLEX_HPP

#include <complex> // complex

#include <SF/Core/Serialization.hpp>

TEMPLATE_SERIALIZATION(save, complex, template <typename ValueType>, std::complex<ValueType>)
{
    auto re = complex.real();
    auto im = complex.imag();

    archive & re & im;
}

TEMPLATE_SERIALIZATION(load, complex, template <typename ValueType>, std::complex<ValueType>)
{
    ValueType re{};
    ValueType im{};

    archive & re & im;

    complex.real(re);
    complex.imag(im);
}

#endif // SF_BUILT_IN_COMPLEX_HPP
