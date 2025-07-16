#include <EightserTestingBase.hpp>

TEST(TestCommon, TestNumber)
{
    static int s_i = 123;
    static long long s_ll = 10101;
    static float s_f = 31.02f;

    static short s_s = -323;
    static double s_d = 2.71;
    static char s_c = 'R';

    static int32_t s_it = 2110033;
    static wchar_t s_wc = L'Q';

    std::vector<unsigned char> storage;
    {
        int i = s_i;
        long long ll = s_ll;
        float f = s_f;

        short s = s_s;
        double d = s_d;
        char c = s_c;

        int32_t it = s_it;
        wchar_t wc = s_wc;

        auto ar = oarchive(storage);

        ar & i & ll & f;
        ar & s & d & c;
        ar & it & wc;
    }
    {
        int i;
        long long ll;
        float f;

        short s;
        double d;
        char c;

        int32_t it;
        wchar_t wc;

        auto ar = iarchive(storage);

        ar & i & ll & f;
        ar & s & d & c;
        ar & it & wc;

        EXPECT("int", i == s_i);
        EXPECT("long long", ll == s_ll);
        EXPECT("float", f == s_f);

        EXPECT("short", s == s_s);
        EXPECT("double", d == s_d);
        EXPECT("char", c == s_c);

        EXPECT("int32_t", it == s_it);
        EXPECT("wchar_t", wc == s_wc);
    }
}
