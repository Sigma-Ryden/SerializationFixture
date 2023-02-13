#include <iostream>

// Base AutoTest class
class AutoTest
{
public:
    static unsigned passed;
    static unsigned failed;

public:
    const char* name;

public:
    AutoTest(const char* name) : name(name) {}

public:
    virtual void Run() = 0;
};

unsigned AutoTest::passed = 0;
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
        passed += ok;                                                                                   \
        failed += !ok;                                                                                  \
        std::cout << name << ". " __VA_ARGS__ << " - " << (ok?"Ok":"Fail") << '\n';                     \
    } while(false)

#define STAT_AUTO_TEST(...)                                                                             \
    std::cout << "\nPassed: " << AutoTest::passed << "\nFailed: " << AutoTest::failed << '\n'

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

        ASSERT(i == s_i && f == s_f, "Enum");
        ASSERT(r == s_r && w == s_w && b == s_b, "Scoped Enum");
    }
}

struct Vector
{
    //SERIALIZABLE(Vector) // not required

    Vector(float x = 0.f, float y = 0.f, float z = 0.f)
        : X(x), Y(y), Z(z) {}

    float X;
    float Y;
    float Z;
};

struct Box
{
    //SERIALIZABLE(Box) // not required

    Box() {}
    Box(const Vector& min, const Vector& max)
        : Min(min), Max(max) {}

    Vector Min;
    Vector Max;
};

SERIALIZATION(SaveLoad, Vector)
{
    archive & self.X & self.Y & self.Z;
}

SERIALIZATION(SaveLoad, Box)
{
    archive & self.Min & self.Max;
}

DEFINE_AUTO_TEST(TestUserType)
{
    static Vector s_min(0.1f, 1.3f, 2.1f);
    static Vector s_max(3.2f, 2.f, 3.5f);

    static auto is_equal = [](const Vector& A, const Vector& B)
    {
        return A.X == B.X && A.Y == B.Y && A.Z == B.Z;
    };

    std::vector<unsigned char> storage;
    {
        Box box(s_min, s_max);

        auto ar = oarchive<OByteStream>(storage);
        ar & box;
    }
    {
        Box box;

        auto ar = iarchive<IByteStream>(storage);
        ar & box;

        ASSERT(is_equal(box.Min, s_min) && is_equal(box.Max, s_max), "Struct");
    }
}

#include <Siraf/Support/array.hpp>

DEFINE_AUTO_TEST(TestArray)
{
    constexpr auto s_a_size = 5;
    static std::array<char, s_a_size> s_a = { 'H', 'e', 'l', 'l', 'o' };

    std::vector<unsigned char> storage;
    {
        std::array<char, s_a_size> a = s_a;

        auto ar = oarchive<OByteStream>(storage);
        ar & a;
    }
    {
        std::array<char, s_a_size> a = s_a;

        auto ar = iarchive<IByteStream>(storage);
        ar & a;

        ASSERT(std::equal(a.begin(), a.end(), s_a.begin()), "std::array<>");
    }
}

#include <Siraf/Support/bitset.hpp>

DEFINE_AUTO_TEST(TestBitset)
{
    static auto s_value_8 = 0b001010001101;

    std::vector<unsigned char> storage;
    {
        std::bitset<12> b_8 = s_value_8;

        auto ar = oarchive<OByteStream>(storage);
        ar & b_8;
    }
    {
        std::bitset<12> b_8;

        auto ar = iarchive<IByteStream>(storage);
        ar & b_8;

        ASSERT(b_8==s_value_8, "std::bitset<>");
    }
}

#include <Siraf/Support/deque.hpp>

DEFINE_AUTO_TEST(TestDeque)
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

        ASSERT(d.size() == s_d.size(), "std::deque<>");
        ASSERT(std::equal(d.begin(), d.end(), s_d.begin()), "std::deque<>");
    }
}

#include <Siraf/Support/forward_list.hpp>

DEFINE_AUTO_TEST(TestForwardList)
{
    enum class State { Free, Blocked, Forced };

    static std::forward_list<State> s_fl = { State::Forced, State::Blocked, State::Forced, State::Free };
    static auto s_fl_size = std::distance(s_fl.begin(), s_fl.end());

    std::vector<unsigned char> storage;
    {
        std::forward_list<State> fl = s_fl;

        auto ar = oarchive<OByteStream>(storage);
        ar & fl;
    }
    {
        std::forward_list<State> fl;

        auto ar = iarchive<IByteStream>(storage);
        ar & fl;

        ASSERT(std::distance(fl.begin(), fl.end()) == s_fl_size, "std::forward_list<>");
        ASSERT(std::equal(fl.begin(), fl.end(), s_fl.begin()), "std::forward_list<>");
    }
}

#include <Siraf/Support/list.hpp>

DEFINE_AUTO_TEST(TestList)
{
    enum Color
    {
        R = 0xff0000, G = 0x00ff00, B = 0x0000ff
    };

    static std::list<Color> s_l = { Color(Color::R | Color::B), Color::G, Color::R };

    std::vector<unsigned char> storage;
    {
        std::list<Color> l = s_l;

        auto ar = oarchive<OByteStream>(storage);
        ar & l;
    }
    {
        std::list<Color> l;

        auto ar = iarchive<IByteStream>(storage);
        ar & l;

        ASSERT(l.size() == s_l.size(), "std::list<>");
        ASSERT(std::equal(l.begin(), l.end(), s_l.begin()), "std::list<>");
    }
}

#include <Siraf/Support/pair.hpp>

struct IntPoint
{
    int X;
    int Y;
};

constexpr bool operator== (const IntPoint& A, const IntPoint& B) noexcept
{
    return A.X == B.X && A.Y == B.Y;
}

SERIALIZATION(SaveLoad, IntPoint)
{
    archive & self.X & self.Y;
}

DEFINE_AUTO_TEST(TestPair)
{
    std::pair<std::uintptr_t, IntPoint> s_p = {0xff00fdda0bacca0f, {256, -128}};

    std::vector<unsigned char> storage;
    {
        std::pair<std::uintptr_t, IntPoint> p = s_p;

        auto ar = oarchive<OByteStream>(storage);
        ar & p;
    }
    {
        std::pair<std::uintptr_t, IntPoint> p;

        auto ar = iarchive<IByteStream>(storage);
        ar & p;

        ASSERT(p == s_p, "std::pair<>");
    }
}

#include <Siraf/Support/vector.hpp>

DEFINE_AUTO_TEST(TestVector)
{
    static std::vector<bool> s_bv = { true, true, false, true };
    static std::vector<double> s_dv = { 17.85, 211.2, 8.723 };

    std::vector<unsigned char> storage;
    {
        std::vector<bool> bv = s_bv;
        std::vector<double> dv = s_dv;

        auto ar = oarchive<OByteStream>(storage);
        ar & bv & dv;
    }
    {
        std::vector<bool> bv;
        std::vector<double> dv;

        auto ar = iarchive<IByteStream>(storage);
        ar & bv & dv;

        ASSERT(bv.size() == s_bv.size(), "std::vector<bool>");
        ASSERT(std::equal( bv.begin(), bv.end(), s_bv.begin() ), "std::vector<bool>");

        ASSERT(dv.size() == s_dv.size(), "std::vector<>");
        ASSERT(std::equal( dv.begin(), dv.end(), s_dv.begin() ), "std::vector<>");
    }
}

int main()
{
    RUN_AUTO_TEST(TestNumber);
    RUN_AUTO_TEST(TestEnum);
    RUN_AUTO_TEST(TestUserType);

    RUN_AUTO_TEST(TestArray);
    RUN_AUTO_TEST(TestBitset);
    RUN_AUTO_TEST(TestDeque);
    RUN_AUTO_TEST(TestForwardList);
    RUN_AUTO_TEST(TestList);
    RUN_AUTO_TEST(TestPair);
    RUN_AUTO_TEST(TestVector);

    STAT_AUTO_TEST();

    return 0;
}
