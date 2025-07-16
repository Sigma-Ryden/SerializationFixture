#include <EightserTestingBase.hpp>

#include <Eightser/BuiltIn/atomic.hpp>

TEST(TestConcurrency, TestAtomic)
{
    static unsigned sv_a = 626126;

    std::vector<unsigned char> storage;
    {
        std::atomic<unsigned> a(sv_a);

        auto ar = oarchive(storage);
        ar & a;
    }
    {
        std::atomic<unsigned> a;

        auto ar = iarchive(storage);
        ar & a;

        EXPECT("std::atomic<>", a.load() == sv_a);
    }
}
