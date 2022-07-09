#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::ReadArchive;
using sifar::WriteArchive;

using sifar::base;
using sifar::virtual_base;

using sifar::utility::pure;

using namespace sifar::common; // support of common types
using namespace sifar::tracking; // support of data tracking

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

struct B
{
    int b = 0;
    virtual ~B() = default;

    SERIALIZATION_POLYMORPHIC_KEY(0)
    SERIALIZATION_UNIFIED(ar)
    {
        ar & b;
    }
};

struct C : virtual B
{
    int c = 1;
    SERIALIZATION_POLYMORPHIC_KEY(1)
    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<B>(ar, *this);
        ar & c;
    }
};

struct D : virtual B
{
    int d = 22;
    SERIALIZATION_POLYMORPHIC_KEY(2)
    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<B>(ar, *this);
        ar & d;
    }
};

struct X : C, D
{
    int x = 333;
    SERIALIZATION_POLYMORPHIC_KEY(3)
    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<B>(ar, *this);
        base<C>(ar, *this);
        base<D>(ar, *this);

        ar & x;
    }
};

SERIALIZATION_EXPORT_KEY(B)
SERIALIZATION_EXPORT_KEY(C)
SERIALIZATION_EXPORT_KEY(D)
SERIALIZATION_EXPORT_KEY(X)

void test_tracking_virtual()
{
    using Registry = sifar::InnerRegistry<B, C, D, X>;

    {
        std::ofstream file("test_tracking_virtual.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = WriteArchive<std::ofstream, Registry>(file);

        X* x = new X;
        D* d = x;
        C* c = x;
        B* b = x;

        println(x);
        println(d);
        println(c);
        println(b);

        std::cout << '\n';

        println(pure(x));
        println(pure(d));
        println(pure(c));
        println(pure(b));

        try
        {
            track(ar, x);
            track(ar, d);
            track(ar, c);
            track(ar, b);
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }
    }
    std::cout << "---\n";
    {
        std::ifstream file("test_tracking_virtual.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = ReadArchive<std::ifstream>(file);

        X* x = nullptr;
        D* d = nullptr;
        C* c = nullptr;
        B* b = nullptr;

        try
        {
            track(ar, x);
            track(ar, d);
            track(ar, c);
            track(ar, b);
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        println(x);
        println(d);
        println(c);
        println(b);

        std::cout << '\n';

        println(pure(x));
        println(pure(d));
        println(pure(c));
        println(pure(b));
    }
}

int main()
{
    test_tracking_virtual();

    return 0;
}
