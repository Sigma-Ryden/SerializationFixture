#include <SFTestingBase.hpp>

#include <SerializationFixture/BuiltIn/stack.hpp>

TEST(TestSTLAdapter, TestStack)
{
    std::stack<char16_t> s_s;
    s_s.push(u's');
    s_s.push(u't');
    s_s.push(u'a');
    s_s.push(u'c');
    s_s.push(u'k');

    std::vector<unsigned char> storage;
    {
        std::stack<char16_t> s = s_s;

        auto ar = oarchive(storage);
        ar & s;
    }
    {
        std::stack<char16_t> s;

        auto ar = iarchive(storage);
        ar & s;

        EXPECT("std::stack<>", s == s_s);
    }
}
