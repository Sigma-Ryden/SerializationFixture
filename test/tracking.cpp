#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Serialization/Core.hpp>
#include <Serialization/Support/string.hpp>

namespace sr = serialization;

using namespace sr::common; // support of common types
using namespace sr::tracking; // support of data tracking

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_tracking()
{
    {
        std::ofstream file("D:/tracking.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = sr::WriteArchive<std::ofstream>(file);

        int x = 123;
        int* p1 = &x;
        int* p2 = p1;

        println(x);
        println(p1);
        println(p2);
        println(*p1);
        println(*p2);

        track(ar, x); // track data and read/write with key
        track_partial(ar, p1); // track data if not track and read/write with key
        track_always(ar, p2); // track data and read/write key even if it first track
    }
    {
        std::ifstream file("D:/tracking.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = sr::ReadArchive<std::ifstream>(file);

        int x;
        int* p1;
        int* p2;

        track(ar, x);
        track_partial(ar, p1);
        track_always(ar, p2);

        println(x);
        println(p1);
        println(p2);
        println(*p1);
        println(*p2);
    }
}

int main()
{
    test_tracking();

    return 0;
}
