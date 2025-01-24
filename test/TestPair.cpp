#include <SFTestingBase.hpp>

#include <SerializationFixture/BuiltIn/pair.hpp>

TEST_SPACE()
{

struct IntPoint
{
    int X;
    int Y;
};

constexpr bool operator== (const IntPoint& A, const IntPoint& B) noexcept
{
    return A.X == B.X && A.Y == B.Y;
}

} // TEST_SPACE

SERIALIZABLE_DECLARATION(IntPoint)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, IntPoint)
    SERIALIZATION
    (
        archive & self.X & self.Y;
    )
SERIALIZABLE_INIT()

TEST(TestUtility, TestPair)
{
    static std::pair<std::uintptr_t, IntPoint> s_p = {0xfedcba, {256, -128}};

    std::vector<unsigned char> storage;
    {
        std::pair<std::uintptr_t, IntPoint> p = s_p;

        auto ar = oarchive(storage);
        ar & p;
    }
    {
        std::pair<std::uintptr_t, IntPoint> p;

        auto ar = iarchive(storage);
        ar & p;

        EXPECT("std::pair<>", p == s_p);
    }
}
