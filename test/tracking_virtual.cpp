#include <iostream> // cout
#include <fstream> // ifstream, ofstream

#define SIFAR_DEBUG
#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::reader;
using sifar::writer;

using sifar::virtual_base;
using sifar::hierarchy;
using sifar::native_base;

using sifar::memory::pure;

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

// Needed for clonable and serializable of derived object
struct B : POLYMORPHIC_BASE()
{
    POLYMORPHIC(B)

    int b = 0;
};
SAVE_LOAD_SERIALIZABLE(B)
{
    archive & self.b;
}

struct C : virtual B
{
    SERIALIZABLE_POLYMORPHIC(C)

    int c = 1;
};
SAVE_LOAD_SERIALIZABLE(C)
{
    archive & virtual_base<B>(self)
            & self.c;
}

struct D : virtual B
{
    POLYMORPHIC(D) // same as SERIALIZABLE_POLYMORPHIC()

    int d = 22;
};

SERIALIZATION(D) // same as: SAVE_LOAD_SERIALIZABLE(D) {...}
{
    archive & native_base<B>(self) // expand to => virtual_base<B>
            & self.d;
}

struct X : C, D
{
    POLYMORPHIC(X)

    int x = 333;
};

SAVE_LOAD_SERIALIZABLE(X)
{
    archive & hierarchy<B, C, D>(self) // expand to => virtual_base<B>, base<C>, base<D>
            & self.x;
}

void test_tracking_virtual()
{
    {
        std::ofstream file("test_tracking_virtual.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = writer(file);

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
        { // special shuffle
            ar & d
               & b
               & c
               & x;
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

        auto ar = reader(file);

        X* x = nullptr;
        D* d = nullptr;
        C* c = nullptr;
        B* b = nullptr;

        try
        { // special shuffle
            ar & c
               & d
               & x
               & b;
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
