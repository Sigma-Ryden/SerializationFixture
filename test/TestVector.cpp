#include <EightserTestingBase.hpp>

#include <Eightser/BuiltIn/vector.hpp>

TEST(TestSTL, TestVector)
{
    static std::vector<bool> s_bv = { true, true, false, true };
    static std::vector<double> s_dv = { 17.85, 211.2, 8.723 };

    std::vector<unsigned char> storage;
    {
        std::vector<bool> bv = s_bv;
        std::vector<double> dv = s_dv;

        auto ar = oarchive(storage);
        ar & bv & dv;
    }
    {
        std::vector<bool> bv;
        std::vector<double> dv;

        auto ar = iarchive(storage);
        ar & bv & dv;

        EXPECT("std::vector<bool>", bv == s_bv);
        EXPECT("std::vector<>", dv == s_dv);
    }
}
