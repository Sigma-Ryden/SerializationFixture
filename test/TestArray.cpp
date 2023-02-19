#include "TestArray.h"

#include <Siraf/Support/array.hpp>

DEFINE_AUTO_TEST(TestArray)
{
    constexpr auto s_a_size = 5;
    static std::array<char, s_a_size> s_a = { 'H', 'e', 'l', 'l', 'o' };

    std::vector<unsigned char> storage;
    {
        std::array<char, s_a_size> a = s_a;

        auto ar = oarchive<OByteStream>(storage);
        ar & a;
    }
    {
        std::array<char, s_a_size> a = s_a;

        auto ar = iarchive<IByteStream>(storage);
        ar & a;

        EXPECT("std::array<>", a == s_a);
    }
}
