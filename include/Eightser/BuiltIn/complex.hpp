#ifndef EIGHTSER_BUILT_IN_COMPLEX_HPP
#define EIGHTSER_BUILT_IN_COMPLEX_HPP

#include <complex> // complex

#include <Eightser/Core/Serialization.hpp>

TEMPLATE_SERIALIZABLE_DECLARATION(template <typename ValueType>, std::complex<ValueType>)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(save, complex, template <typename ValueType>, std::complex<ValueType>)
    SERIALIZATION
    (
        auto re = complex.real();
        auto im = complex.imag();

        archive & re & im;
    )
SERIALIZABLE_INIT()

TEMPLATE_SERIALIZABLE(load, complex, template <typename ValueType>, std::complex<ValueType>)
    SERIALIZATION
    (
        ValueType re{};
        ValueType im{};

        archive & re & im;

        complex.real(re);
        complex.imag(im);
    )
SERIALIZABLE_INIT()

#endif // EIGHTSER_BUILT_IN_COMPLEX_HPP
