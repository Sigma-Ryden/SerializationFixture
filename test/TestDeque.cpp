#include "SirafTestingBase.hpp"

#include <Siraf/Support/deque.hpp>

TEST(TestSTL, TestDeque)
{
    static std::deque<std::int8_t> s_d = { 1, -2, 3, 0, -2, 5, 4 };

    std::vector<unsigned char> storage;
    {
        std::deque<std::int8_t> d = s_d;

        auto ar = oarchive<OByteStream>(storage);
        ar & d;
    }
    {
        std::deque<std::int8_t> d;

        auto ar = iarchive<IByteStream>(storage);
        ar & d;

        EXPECT("std::deque<>", d.size() == s_d.size());
        EXPECT("std::deque<>", d == s_d);
    }
}