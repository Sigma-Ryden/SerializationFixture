#include "SFTestingBase.hpp"

#include <SF/Support/queue.hpp>

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

        auto ar = oarchive(storage);
        ar & q;
    }
    {
        std::queue<bool> q;

        auto ar = iarchive(storage);
        ar & q;

        EXPECT("std::queue<>", q == s_q);
    }
}
