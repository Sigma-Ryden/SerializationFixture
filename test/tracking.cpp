#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#define SIFAR_DEBUG
#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::reader;
using sifar::writer;

using sifar::tracking::track;

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_tracking()
{
    {
        std::ofstream file("test_tracking.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = writer(file);

        int x = 123;
        int* p1 = &x;
        int* p2 = p1;

        println(&x);
        println(p1);
        println(p2);

        println(x);
        println(*p1);
        println(*p2);

        try
        {
            ar & track(x) // track data and read/write with key
               & p1 // track data if not track and read/write with key
               & p2; // is the same as above
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }
    }
    {
        std::ifstream file("test_tracking.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = reader(file);

        int x;
        int* p1 = nullptr;
        int* p2 = nullptr; // track function will throw exception if pointer != nullptr

        try
        {
            ar & track(x)
               & p1
               & p2;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        println(&x);
        println(p1);
        println(p2);

        println(x);
        println(*p1);
        println(*p2);
    }
}

int main()
{
    test_tracking();

    return 0;
}
