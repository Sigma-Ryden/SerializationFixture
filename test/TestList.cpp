#include "SirafTestingBase.hpp"

#include <Siraf/Support/list.hpp>

TEST(TestSTL, TestList)
{
    enum Color
    {
        R = 0xff0000, G = 0x00ff00, B = 0x0000ff
    };

    static std::list<Color> s_l = { Color(Color::R | Color::B), Color::G, Color::R };

    std::vector<unsigned char> storage;
    {
        std::list<Color> l = s_l;

        auto ar = oarchive(storage);
        ar & l;
    }
    {
        std::list<Color> l;

        auto ar = iarchive(storage);
        ar & l;

        EXPECT("std::list<>", l == s_l);
    }
}
