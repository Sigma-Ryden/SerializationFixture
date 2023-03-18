#include "SirafTestingBase.hpp"

#include <Siraf/Support/shared_ptr.hpp>

using siraf::tracking::track;

TEST(TestLibrary, TestValidation)
{
    std::vector<unsigned char> storage;

    {
        auto ar = oarchive(storage);

        int j = 8379;
        int* p_j = &j;

        ar & p_j;

        auto success = false;
        try { ar & track(j); } catch(...) { success = true; }

        EXPECT("bad data track.write", success);
    }
    {
        auto ar = iarchive(storage);

        int j;
        int* p_j = nullptr;

        ar & p_j;

        bool success = false;
        try { ar & track(j); } catch(...) { success = true; }

        EXPECT("bad data track.read", success);
    }

    {
        auto ar = oarchive(storage);

        int* g = nullptr; // ok - nullptr allowed
        ar & g;
    }
    {
        auto ar = iarchive(storage);

        int* g; // bad, garbage ptr - should be nullptr

        auto success = false;
        try { ar & g; } catch(...) { success = true; }

        EXPECT("garbage ptr.read", success);
    }

    {
        auto ar = oarchive(storage);

        int* m = new int(1277);

        int* r = m; // good
        std::shared_ptr<int> s(m); // bad - mixed tracking

        ar & r & s;
    }
    {
        auto ar = iarchive(storage);

        int* r = nullptr;
        std::shared_ptr<int> s = nullptr;

        ar & r;

        auto success = false;
        try { ar & s; } catch(...) { success = true; }

        EXPECT("mixed track.raw shared", success);
    }

    {
        auto ar = oarchive(storage);

        int* m = new int(8923);

        std::shared_ptr<int> s(m); // god
        int* r = m; // bad - mixed tracking

        ar & s & r;
    }
    {
        auto ar = iarchive(storage);

        std::shared_ptr<int> s = nullptr;
        int* r = nullptr;

        ar & s;

        auto success = false;
        try { ar & r; } catch(...) { success = true; }

        EXPECT("mixed track.shared raw", success);
    }
}
