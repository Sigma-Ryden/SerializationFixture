#include "TestList.h"

#include <Siraf/Support/list.hpp>

DEFINE_AUTO_TEST(TestList)
{
    enum Color
    {
        R = 0xff0000, G = 0x00ff00, B = 0x0000ff
    };

    static std::list<Color> s_l = { Color(Color::R | Color::B), Color::G, Color::R };

    std::vector<unsigned char> storage;
    {
        std::list<Color> l = s_l;

        auto ar = oarchive<OByteStream>(storage);
        ar & l;
    }
    {
        std::list<Color> l;

        auto ar = iarchive<IByteStream>(storage);
        ar & l;

        EXPECT("std::list<>", l.size() == s_l.size());
        EXPECT("std::list<>", l == s_l);
    }
}
