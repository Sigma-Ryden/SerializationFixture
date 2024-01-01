#include "SFTestingBase.hpp"

#include <SF/BuiltIn/queue.hpp>

TEST(TestSTLAdapter, TestPriorityQueue)
{
    const auto sv_pq = {1, 8, 5, 6, 3, 4, 0, 9, 7, 2};

    std::priority_queue<int> s_pq;

    for (auto item : sv_pq)
        s_pq.push(item);

    std::vector<unsigned char> storage;
    {
        std::priority_queue<int> pq = s_pq;

        auto ar = oarchive(storage);
        ar & pq;
    }
    {
        std::priority_queue<int> pq;

        auto ar = iarchive(storage);
        ar & pq;

        auto c = sf::meta::underlying(pq);
        auto s_c = sf::meta::underlying(s_pq);

        EXPECT("std::priority_queue<>", c == s_c);
    }
}
