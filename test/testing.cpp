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

    static auto equal = [](const Vector& A, const Vector& B)
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

        ASSERT(equal(box.Min, s_min) && equal(box.Max, s_max), "Struct");
    }
}

template <class Container>
bool is_equal(const Container& A, const Container& B)
{
    return std::equal(A.begin(), A.end(), B.begin());
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

        ASSERT(is_equal(a, s_a), "std::array<>");
    }
}

#include <Siraf/Support/bitset.hpp>

DEFINE_AUTO_TEST(TestBitset)
{
    static auto s_bvalue_12 = 0b001010001101;

    std::vector<unsigned char> storage;
    {
        std::bitset<12> b_12 = s_bvalue_12;

        auto ar = oarchive<OByteStream>(storage);
        ar & b_12;
    }
    {
        std::bitset<12> b_12;

        auto ar = iarchive<IByteStream>(storage);
        ar & b_12;

        ASSERT(b_12==s_bvalue_12, "std::bitset<>");
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
        ASSERT(is_equal(d, s_d), "std::deque<>");
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
        ASSERT(is_equal(fl, s_fl), "std::forward_list<>");
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
        ASSERT(is_equal(l, s_l), "std::list<>");
    }
}

#include <Siraf/Support/map.hpp>

DEFINE_AUTO_TEST(TestMap)
{
    enum class Hint { Alpha, Relative, Fly };

    static std::map<int, float> s_m = { {3, 0.1f}, {0, 0.5f}, {4, 0.17f} };
    static std::unordered_map<int, char> s_um = { {1, 'c'}, {3, '+'}, {2, '+'} };
    static std::multimap<Hint, int> s_mm = { {Hint::Fly, -1}, {Hint::Alpha, 1}, {Hint::Fly, 0} };
    static std::unordered_multimap<int, int> s_umm = { {1, -1}, {-1, 0}, {1, 0}, {1, 2}, {-1, 2} };

    std::vector<unsigned char> storage;
    {
        std::map<int, float> m = s_m;
        std::unordered_map<int, char> um = s_um;
        std::multimap<Hint, int> mm = s_mm;
        std::unordered_multimap<int, int> umm = s_umm;

        auto ar = oarchive<OByteStream>(storage);
        ar & m & um & mm & umm;
    }
    {
        std::map<int, float> m;
        std::unordered_map<int, char> um;
        std::multimap<Hint, int> mm;
        std::unordered_multimap<int, int> umm;

        auto ar = iarchive<IByteStream>(storage);
        ar & m & um & mm & umm;

        //ASSERT(m.size() == s_m_size, "std::map<>");
        ASSERT(false, "std::map<>");

        //ASSERT(um.size() == s_um_size, "std::unordered_map<>");
        ASSERT(false, "std::unordered_map<>");

        //ASSERT(mm.size() == s_mm_size, "std::multimap<>");
        ASSERT(false, "std::multimap<>");

        //ASSERT(umm.size() == s_umm_size, "std::unordered_multimap<>");
        ASSERT(false, "std::unordered_multimap<>");
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
    static std::pair<std::uintptr_t, IntPoint> s_p = {0xff00fdda0bacca0f, {256, -128}};

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

#include <Siraf/Support/queue.hpp>

DEFINE_AUTO_TEST(TestQueue)
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

        ASSERT(q.size() == s_q.size(), "std::queue<>");

        auto& c = siraf::detail::underlying(q);
        auto& s_c = siraf::detail::underlying(s_q);

        ASSERT(is_equal(c, s_c), "std::queue<>");
    }
}

#include <Siraf/Support/set.hpp>

DEFINE_AUTO_TEST(TestSet)
{
    ASSERT(false, "std::set<>");
    ASSERT(false, "std::unordered_set<>");
    ASSERT(false, "std::multiset<>");
    ASSERT(false, "std::unordered_multiset<>");
}

#include <Siraf/Support/shared_ptr.hpp>

DEFINE_AUTO_TEST(TestSharedPtr)
{
    ASSERT(false, "std::shared_ptr<>");
}

#include <Siraf/Support/stack.hpp>

DEFINE_AUTO_TEST(TestStack)
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

        ASSERT(s.size() == s_s.size(), "std::stack<>");

        auto& c = siraf::detail::underlying(s);
        auto& s_c = siraf::detail::underlying(s_s);

        ASSERT(is_equal(c, s_c), "std::stack<>");
    }
}

#include <Siraf/Support/string.hpp>

DEFINE_AUTO_TEST(TestString)
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

        auto ar = oarchive<OByteStream>(storage);
        ar & s & ws & u16s & u32s;
    }
    {
        std::string s;
        std::wstring ws;
        std::u16string u16s;
        std::u32string u32s;

        auto ar = iarchive<IByteStream>(storage);
        ar & s & ws & u16s & u32s;

        ASSERT(s == s_s, "std::string");
        ASSERT(ws == s_ws, "std::wstring");
        ASSERT(u16s == s_u16s, "std::u16string");
        ASSERT(u32s == s_u32s, "std::u32string");
    }
}

#include <Siraf/Support/tuple.hpp>

DEFINE_AUTO_TEST(TestTuple)
{
    enum class Charge { Low, Medium, High };

    static std::tuple<> s_et;
    static std::tuple<std::string, double, Charge> s_t{ "Tuple", 123.456, Charge::Low };

    std::vector<unsigned char> storage;
    {
        std::tuple<> et = s_et;
        std::tuple<std::string, double, Charge> t = s_t;

        auto ar = oarchive<OByteStream>(storage);
        ar & et & t;
    }
    {
        std::tuple<> et;
        std::tuple<std::string, double, Charge> t;

        auto ar = iarchive<IByteStream>(storage);
        ar & et & t;

        ASSERT(et == s_et, "std::tuple<>");
        ASSERT(t == s_t, "std::tuple<>");
    }
}

#include <Siraf/Support/unique_ptr.hpp>

DEFINE_AUTO_TEST(TestUniquePtr)
{
    ASSERT(false, "std::unique_ptr<>");
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
        ASSERT(is_equal(bv, s_bv), "std::vector<bool>");

        ASSERT(dv.size() == s_dv.size(), "std::vector<>");
        ASSERT(is_equal(dv, s_dv), "std::vector<>");
    }
}

#include <Siraf/Support/weak_ptr.hpp>

DEFINE_AUTO_TEST(TestWeakPtr)
{
    ASSERT(false, "std::weak_ptr<>");
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
    RUN_AUTO_TEST(TestMap);
    RUN_AUTO_TEST(TestPair);
    RUN_AUTO_TEST(TestQueue);
    RUN_AUTO_TEST(TestSet);
    RUN_AUTO_TEST(TestSharedPtr);
    RUN_AUTO_TEST(TestStack);
    RUN_AUTO_TEST(TestString);
    RUN_AUTO_TEST(TestTuple);
    RUN_AUTO_TEST(TestUniquePtr);
    RUN_AUTO_TEST(TestVector);
    RUN_AUTO_TEST(TestWeakPtr);

    STAT_AUTO_TEST();

    return 0;
}
