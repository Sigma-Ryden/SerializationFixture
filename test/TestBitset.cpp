#include "SirafTestingBase.hpp"

#include <Siraf/Support/bitset.hpp>

TEST(TestSTL, TestBitset)
{
    static auto sv_bitset10 = 653;

    static auto s_position = 100;
    static auto s_value = true;

    std::vector<unsigned char> storage;
    {
        std::bitset<128> b_128;
        b_128.set(s_position, s_value);

        std::bitset<10> b_10 = sv_bitset10;

        auto ar = oarchive<OByteStream>(storage);
        ar & b_10 & b_128;
    }
    {
        std::bitset<128> b_128;
        std::bitset<10> b_10;

        auto ar = iarchive<IByteStream>(storage);
        ar & b_10 & b_128;

        EXPECT("std::bitset<>", b_128.test(s_position) == s_value);
        EXPECT("std::bitset<>", b_10 == sv_bitset10);
    }
}
