#include <EightserTestingBase.hpp>

using eightser::track;

TEST(TestLibrary, TestTracking)
{
    static unsigned sv_i = 28932;

    std::vector<unsigned char> storage;
    {
        int i = sv_i;
        int* p1_i = &i;
        int* p2_i = p1_i;

        auto ar = oarchive(storage);

        ar & track(i) // track data and read/write with key (0)
           & p1_i // track data if not already tracked (1)
           & p2_i; // is the same as above (1)
    }
    {
        int i;
        int* p1_i = nullptr;
        int* p2_i = nullptr; // track function will throw exception if pointer != nullptr

        auto ar = iarchive(storage);

        ar & track(i)
           & p1_i
           & p2_i;

        EXPECT("data track.data value", i == sv_i);
        ASSERT("data track.pointer inited", p1_i != nullptr && p2_i != nullptr);

        EXPECT("data track.pointer value", p1_i == p2_i && p2_i == &i);
        EXPECT("data tarck.value", *p1_i == sv_i && *p2_i == sv_i);
    }
}
