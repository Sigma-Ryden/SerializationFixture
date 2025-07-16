#include <EightserTestingBase.hpp>

TEST(TestCommon, TestEnum)
{
    enum NumberType { None, Int, Float, Char, Bool };
    enum class Color { None, Red, Green, Blue, Black, White };

    NumberType s_i = NumberType::Int;
    NumberType s_f = NumberType::Float;

    Color s_r = Color::Red;
    Color s_b = Color::Blue;
    Color s_w = Color::White;

    std::vector<unsigned char> storage;
    {
        NumberType i = s_i;
        Color r = s_r;

        Color w = s_w;
        NumberType f = s_f;
        Color b = s_b;

        auto ar = oarchive(storage);
        ar & i & r & w & f & b;
    }
    {
        NumberType i;
        Color r;

        Color w;
        NumberType f;
        Color b;

        auto ar = iarchive(storage);
        ar & i & r & w & f & b;

        EXPECT("Enum", i == s_i && f == s_f);
        EXPECT("Scoped Enum", r == s_r && w == s_w && b == s_b);
    }
}
