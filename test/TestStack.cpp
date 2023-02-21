#include "SirafTestingBase.hpp"

#include <Siraf/Support/stack.hpp>

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
        auto ar = oarchive<OByteStream>(storage);
        ar & s;
    }
    {
        std::stack<char16_t> s;

        auto ar = iarchive<IByteStream>(storage);
        ar & s;

        EXPECT("std::stack<>", s.size() == s_s.size());

        auto& c = siraf::detail::underlying(s);
        auto& s_c = siraf::detail::underlying(s_s);

        EXPECT("std::stack<>", c == s_c);
    }
}
