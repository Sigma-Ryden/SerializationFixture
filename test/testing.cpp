#include <iostream>

// Base AutoTest class
struct TestModule
{
    static std::ostream& stream;

    static unsigned passed;
    static unsigned failed;

    static void Expect(const char* name, const char* text, bool condition)
    {
        passed += condition;
        failed += !condition;
        stream << name << ". " << text << " - " << (condition?"Ok":"Fail") << '\n';
    }

    static void Stat()
    {
        stream << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
    }
};

std::ostream& TestModule::stream = std::cout;

unsigned TestModule::passed = 0;
unsigned TestModule::failed = 0;

// Helpful macros:
// Define your own impl or Run test function
#define TEST(test, ...)                                                                                 \
    struct test { test(); const char* name = #test; };                                                  \
    static std::unique_ptr<test> s##test(new test);                                                     \
    test::test()

// Calculate completed and failed test, provide debug info
#define EXPECT(msg, ...)                                                                                \
    TestModule::Expect(name, msg, __VA_ARGS__)

// Your own tests bellow:

// Core minimal
#include <Siraf/Core.hpp>

using siraf::oarchive;
using siraf::wrapper::OByteStream;

using siraf::iarchive;
using siraf::wrapper::IByteStream;

using siraf::hierarchy;
// ~

TEST(TestNumber)
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

TEST(TestEnum)
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

        EXPECT("Enum", i == s_i && f == s_f);
        EXPECT("Scoped Enum", r == s_r && w == s_w && b == s_b);
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

TEST(TestUserType)
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

        EXPECT("Struct", equal(box.Min, s_min) && equal(box.Max, s_max));
    }
}

#include <Siraf/Support/array.hpp>

TEST(TestArray)
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

        EXPECT("std::array<>", a == s_a);
    }
}

#include <Siraf/Support/bitset.hpp>

TEST(TestBitset)
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

        EXPECT("std::bitset<>", b_12 == s_bvalue_12);
    }
}

#include <Siraf/Support/deque.hpp>

TEST(TestDeque)
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

        EXPECT("std::deque<>", d.size() == s_d.size());
        EXPECT("std::deque<>", d == s_d);
    }
}

#include <Siraf/Support/forward_list.hpp>

TEST(TestForwardList)
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

        EXPECT("std::forward_list<>", std::distance(fl.begin(), fl.end()) == s_fl_size);
        EXPECT("std::forward_list<>", fl == s_fl);
    }
}

#include <Siraf/Support/list.hpp>

TEST(TestList)
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

        EXPECT("std::list<>", l.size() == s_l.size());
        EXPECT("std::list<>", l == s_l);
    }
}

#include <Siraf/Support/map.hpp>

template <typename Tree>
bool is_tree_equal(const Tree& lhs, const Tree& rhs)
{
    if (lhs.size() != rhs.size()) return false;

    for(auto& pair:rhs) if(lhs.count(pair.first) == 0) return false;
    return true;
}

TEST(TestMap)
{
    enum class Hint { Alpha, Relative, Fly };

    static std::map<int, float> s_m = { {0, 0.5f}, {3, 0.1f}, {4, 0.17f} };
    static std::unordered_map<int, char> s_um = { {1, 'c'}, {2, '+'}, {3, '+'} };
    static std::multimap<Hint, int> s_mm = { {Hint::Alpha, 1}, {Hint::Fly, -1}, {Hint::Fly, 0} };
    static std::unordered_multimap<int, int> s_umm = { {-1, 0}, {-1, 2}, {1, -1}, {1, 0}, {1, 2} };

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

        EXPECT("std::map<>", is_tree_equal(m, s_m));

        EXPECT("std::unordered_map<>", um.size() == s_um.size());
        EXPECT("std::unordered_map<>", um == s_um);

        EXPECT("std::multimap<>", is_tree_equal(mm, s_mm));

        EXPECT("std::unordered_multimap<>", umm.size() == s_umm.size());
        EXPECT("std::unordered_multimap<>", umm == s_umm);
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

TEST(TestPair)
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

        EXPECT("std::pair<>", p == s_p);
    }
}

#include <Siraf/Support/queue.hpp>

TEST(TestQueue)
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

        EXPECT("std::queue<>", q.size() == s_q.size());

        auto& c = siraf::detail::underlying(q);
        auto& s_c = siraf::detail::underlying(s_q);

        EXPECT("std::queue<>", c == s_c);
    }
}

#include <Siraf/Support/set.hpp>

TEST(TestSet)
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

        auto ar = oarchive<OByteStream>(storage);
        ar & s & us & ms & ums;
    }
    {
        std::set<int> s;
        std::unordered_set<char> us;
        std::multiset<Signal> ms;
        std::unordered_multiset<int> ums;

        auto ar = iarchive<IByteStream>(storage);
        ar & s & us & ms & ums;

        EXPECT("std::set<>", s.size() == s_s.size());
        EXPECT("std::set<>", s == s_s);

        EXPECT("std::unordered_set<>", us.size() == s_us.size());
        EXPECT("std::unordered_set<>", us == s_us);

        EXPECT("std::multiset<>", ms.size() == s_ms.size());
        EXPECT("std::multiset<>", ms == s_ms);

        EXPECT("std::unordered_multiset<>", ums.size() == s_ums.size());
        EXPECT("std::unordered_multiset<>", ums == s_ums);
    }
}

#include <Siraf/Support/shared_ptr.hpp>

TEST(TestSharedPtr)
{
    EXPECT("std::shared_ptr<>", false);
}

#include <Siraf/Support/stack.hpp>

TEST(TestStack)
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

#include <Siraf/Support/string.hpp>

TEST(TestString)
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

        EXPECT("std::string", s == s_s);
        EXPECT("std::wstring", ws == s_ws);
        EXPECT("std::u16string", u16s == s_u16s);
        EXPECT("std::u32string", u32s == s_u32s);
    }
}

#include <Siraf/Support/tuple.hpp>

TEST(TestTuple)
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

        EXPECT("std::tuple<empty>", et == s_et);
        EXPECT("std::tuple<>", t == s_t);
    }
}

#include <Siraf/Support/unique_ptr.hpp>

struct Parent : POLYMORPHIC()
{
    SERIALIZABLE(Parent)

    virtual ~Parent()=default;
    int p = 0;
};

SERIALIZATION(SaveLoad, Parent)
{
    archive & self.p;
}

struct Child : Parent
{
    SERIALIZABLE(Child)

    int c = 0;
};

SERIALIZATION(SaveLoad, Child)
{
    archive & hierarchy<Parent>(self) & self.c;
}

bool operator== (const Child& lhs, const Child& rhs)
{
    return lhs.p == rhs.p && lhs.c == rhs.c;
}

TEST(TestUniquePtr)
{
    static int sv_i = 8791;

    Child sv_c;
    sv_c.p = 506;
    sv_c.c = 1234;

    std::vector<unsigned char> storage;
    {
        std::unique_ptr<int> u_i(new int(sv_i));
        std::unique_ptr<Parent> u_p(new Child(sv_c));

        auto ar = oarchive<OByteStream>(storage);
        ar & u_i & u_p;
    }
    {
        std::unique_ptr<int> u_i = nullptr;
        std::unique_ptr<Parent> u_p = nullptr;

        auto ar = iarchive<IByteStream>(storage);
        ar & u_i & u_p;

        EXPECT("std::unique_ptr<>", u_i != nullptr && *u_i == sv_i);

        auto raw_u_p = u_p.get();
        auto raw_u_c = dynamic_cast<Child*>(raw_u_p);

        EXPECT("std::unique_ptr<polymorphic>", raw_u_p != nullptr && raw_u_c != nullptr && *raw_u_c == sv_c);
    }
}

#include <Siraf/Support/vector.hpp>

TEST(TestVector)
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

        EXPECT("std::vector<bool>", bv.size() == s_bv.size());
        EXPECT("std::vector<bool>", bv == s_bv);

        EXPECT("std::vector<>", dv.size() == s_dv.size());
        EXPECT("std::vector<>", dv == s_dv);
    }
}

#include <Siraf/Support/weak_ptr.hpp>

TEST(TestWeakPtr)
{
    static int sv_i = 983258;

    Child sv_c;
    sv_c.p = 7458;
    sv_c.c = 589;

    std::vector<unsigned char> storage;
    {
        auto s_i = std::make_shared<int>(sv_i);
        auto s_p = std::make_shared<Child>(sv_c);

        std::weak_ptr<int> w_i = s_i;
        std::weak_ptr<Parent> w_p = s_p;

        auto ar = oarchive<OByteStream>(storage);
        ar & w_i & w_p;
    }
    {
        std::unique_ptr<int> w_i = nullptr;
        std::unique_ptr<Parent> w_p = nullptr;

        auto ar = iarchive<IByteStream>(storage);
        ar & w_i & w_p;

        EXPECT("std::weak_ptr<>", w_i != nullptr && *w_i == sv_i);

        auto raw_w_p = w_p.get();
        auto raw_w_c = dynamic_cast<Child*>(raw_w_p);

        EXPECT("std::weak_ptr<polymorphic>", raw_w_p != nullptr && raw_w_c != nullptr && *raw_w_c == sv_c);
    }
}

int main()
{
    TestModule::Stat();
    return 0;
}
