#include "SirafTestingBase.hpp"

#include <Siraf/Support/queue.hpp>

TEST(TestSTLAdapter, TestQueue)
{
    std::queue<bool> s_q;
    s_q.push(true);
    s_q.push(false);
    s_q.push(true);
    s_q.push(false);
    s_q.push(false);

    std::vector<unsigned char> storage;
    {
        std::queue<bool> q = s_q;
        auto ar = oarchive<OByteStream>(storage);
        ar & q;
    }
    {
        std::queue<bool> q;

        auto ar = iarchive<IByteStream>(storage);
        ar & q;

        EXPECT("std::queue<>", q.size() == s_q.size());

        auto& c = siraf::detail::underlying(q);
        auto& s_c = siraf::detail::underlying(s_q);

        EXPECT("std::queue<>", c == s_c);
    }
}
