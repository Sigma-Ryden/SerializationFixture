#include <EightserTestingBase.hpp>

#include <Eightser/BuiltIn/complex.hpp>

TEST(TestNumeric, TestComplex)
{
    static std::complex<double> s_c = {1.0, -0.5};

    std::vector<unsigned char> storage;
    {
        std::complex<double> c = s_c;

        auto ar = oarchive(storage);
        ar & c;
    }
    {
         std::complex<double> c;

        auto ar = iarchive(storage);
        ar & c;

        EXPECT("std::complex<>", c == s_c);
    }
}
