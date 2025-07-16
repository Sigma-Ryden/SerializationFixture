#include <EightserTestingBase.hpp>

#include <Eightser/BuiltIn/string.hpp>

TEST(TestSTL, TestString)
{
    static std::string s_s = "Hello, World!";
    static std::wstring s_ws = L"You're welcome!";
    static std::u16string s_u16s = u"Stack overflow";
    static std::u32string s_u32s = U"Github.com";

    std::vector<unsigned char> storage;
    {
        std::string s = s_s;
        std::wstring ws = s_ws;
        std::u16string u16s = s_u16s;
        std::u32string u32s = s_u32s;

        auto ar = oarchive(storage);
        ar & s & ws & u16s & u32s;
    }
    {
        std::string s;
        std::wstring ws;
        std::u16string u16s;
        std::u32string u32s;

        auto ar = iarchive(storage);
        ar & s & ws & u16s & u32s;

        EXPECT("std::string", s == s_s);
        EXPECT("std::wstring", ws == s_ws);
        EXPECT("std::u16string", u16s == s_u16s);
        EXPECT("std::u32string", u32s == s_u32s);
    }
}
