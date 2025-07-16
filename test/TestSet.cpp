#include <EightserTestingBase.hpp>

#include <Eightser/BuiltIn/set.hpp>

TEST(TestSTL, TestSet)
{
    enum class Signal { True, False };

    static std::set<int> s_s = {1, 2, 3, 2, 3, 2, 1, 0, 1, 4};
    static std::unordered_set<char> s_us = { 'u', 'n', 'o', 'r', 'd', 'e', 'r', 'e', 'd', 's', 'e', 't' };
    static std::multiset<Signal> s_ms = { Signal::True, Signal::False, Signal::True };
    static std::unordered_multiset<int> s_ums = { -1, -1, 1, 1, 0, 1, -1, 0 };

    std::vector<unsigned char> storage;
    {
        std::set<int> s = s_s;
        std::unordered_set<char> us = s_us;
        std::multiset<Signal> ms = s_ms;
        std::unordered_multiset<int> ums = s_ums;

        auto ar = oarchive(storage);
        ar & s & us & ms & ums;
    }
    {
        std::set<int> s;
        std::unordered_set<char> us;
        std::multiset<Signal> ms;
        std::unordered_multiset<int> ums;

        auto ar = iarchive(storage);
        ar & s & us & ms & ums;

        EXPECT("std::set<>", s == s_s);
        EXPECT("std::unordered_set<>", us == s_us);

        EXPECT("std::multiset<>", ms == s_ms);
        EXPECT("std::unordered_multiset<>", ums == s_ums);
    }
}
