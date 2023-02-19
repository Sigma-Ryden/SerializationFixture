#include "TestBitset.h"

#include <Siraf/Support/bitset.hpp>

DEFINE_AUTO_TEST(TestBitset)
{
    static auto s_bvalue_12 = 653;

    std::vector<unsigned char> storage;
    {
        std::bitset<12> b_12 = s_bvalue_12;

        auto ar = oarchive<OByteStream>(storage);
        ar & b_12;
    }
    {
        std::bitset<12> b_12;

        auto ar = iarchive<IByteStream>(storage);
        ar & b_12;

        EXPECT("std::bitset<>", b_12 == s_bvalue_12);
    }
}
