#include <iostream>

// Base AutoTest class
class AutoTest
{
public:
    static unsigned completed;
    static unsigned failed;

public:
    const char* name;

public:
    AutoTest(const char* name) : name(name) {}

public:
    virtual void Run() = 0;
};

unsigned AutoTest::completed = 0;
unsigned AutoTest::failed = 0;

// Helpful macros:
// Define your own impl or Run test function
#define DEFINE_AUTO_TEST(class_name)                                                                    \
    class class_name : public AutoTest {                                                                \
    public:                                                                                             \
        class_name() : AutoTest(#class_name) {}                                                         \
        virtual void Run() override;                                                                    \
    };                                                                                                  \
    void class_name::Run()

// Run four own test impl defined before
#define RUN_AUTO_TEST(class_name)                                                                       \
    class_name().Run()

// Calculate completed and failed test, provide debug info
#define ASSERT(condition, ...)                                                                          \
    do {                                                                                                \
        auto ok = (condition);                                                                          \
        completed += ok;                                                                                \
        failed += !ok;                                                                                  \
        std::cout << name << ". " __VA_ARGS__ << " - " << (ok?"Ok":"Fail") << '\n';                     \
    } while(false)

#define STATS_AUTO_TEST(...)                                                                            \
    std::cout << "\nCompleted: " << AutoTest::completed << "\nFailed: " << AutoTest::failed << '\n'

// Your own tests bellow:

// Core minimal
#include <Siraf/Core.hpp>

using siraf::oarchive;
using siraf::wrapper::OByteStream;

using siraf::iarchive;
using siraf::wrapper::IByteStream;
// ~

DEFINE_AUTO_TEST(TestNumber)
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

        auto ar = oarchive<OByteStream>(storage);

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

        auto ar = iarchive<IByteStream>(storage);

        ar & i & ll & f;
        ar & s & d & c;
        ar & it & wc;

        ASSERT(i == s_i, "int");
        ASSERT(ll == s_ll, "long long");
        ASSERT(f == s_f, "float");

        ASSERT(s == s_s, "short");
        ASSERT(d == s_d, "double");
        ASSERT(c == s_c, "char");

        ASSERT(it == s_it, "int32_t");
        ASSERT(wc == s_wc, "wchar_t");
    }
}

DEFINE_AUTO_TEST(TestEnum)
{
    enum NumberType { None, Int, Float, Char, Bool };
    enum class Color { None, Red, Green, Blue, Black, White };

    NumberType s_i = NumberType::Int;
    NumberType s_f = NumberType::Float;

    Color s_r = Color::Red;
    Color s_b = Color::Blue;
    Color s_w = Color::White;

    std::vector<unsigned char> storage;
    {
        NumberType i = s_i;
        Color r = s_r;

        Color w = s_w;
        NumberType f = s_f;
        Color b = s_b;

        auto ar = oarchive<OByteStream>(storage);
        ar & i & r & w & f & b;
    }
    {
        NumberType i;
        Color r;

        Color w;
        NumberType f;
        Color b;

        auto ar = iarchive<IByteStream>(storage);
        ar & i & r & w & f & b;

        ASSERT(i == s_i && f == s_f, "NumberType");
        ASSERT(r == s_r && w == s_w && b == s_b, "Color");
    }
}

int main()
{
    RUN_AUTO_TEST(TestNumber);
    RUN_AUTO_TEST(TestEnum);

    STATS_AUTO_TEST();
    return 0;
}
