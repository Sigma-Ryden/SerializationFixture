#include "SFTestingBase.hpp"

#include <SF/BuiltIn/array.hpp>

TEST_SPACE()
{

union CommonData
{
    int i;
    double d;
    char c;
};


union Variant
{
    CommonData* p;
    std::array<wchar_t, 3> a;
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, Variant)
{
    // specify impl here
    archive & self.a;
}

TEST(TestCommon, TestUnion)
{
    static double s_cdd = 2.71;
    static std::array<wchar_t, 3> s_a = { L'C', L'+', L'+' };

    std::vector<unsigned char> storage;
    {
        CommonData cd;
        Variant v;

        cd.d = s_cdd;
        v.a = s_a;

        auto ar = oarchive(storage);
        ar & cd & v;
    }
    {
        CommonData cd;
        Variant v;

        auto ar = iarchive(storage);
        ar & cd & v;

        EXPECT("union binary", cd.d == s_cdd);
        EXPECT("union", v.a == s_a);
    }
}
