#include "TestPair.h"

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

        EXPECT("std::pair<>", p == s_p);
    }
}
