#include <iostream> // cout
#include <fstream> // ifstream, ofstream

#define SIRAF_DEBUG
#include <Siraf/Core.hpp> // ReadArchive, WriteArchive

using siraf::iarchive;
using siraf::oarchive;

using siraf::virtual_base;
using siraf::hierarchy;

using siraf::memory::pure;

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

// Needed for clonable and serializable of derived object
struct B : POLYMORPHIC()
{
    SERIALIZABLE(B)

    int b = 0;
};
SAVE_LOAD_SERIALIZABLE(B)
{
    archive & self.b;
}

struct C : virtual B
{
    SERIALIZABLE(C)

    int c = 1;
};
SAVE_LOAD_SERIALIZABLE(C)
{
    archive & virtual_base<B>(self)
            & self.c;
}

struct D : virtual B
{
    SERIALIZABLE(D) // same as SERIALIZABLE_POLYMORPHIC()

    int d = 22;
};

SERIALIZATION(D) // same as: SAVE_LOAD_SERIALIZABLE(D) {...}
{
    archive & hierarchy<B>(self) // expand to => virtual_base<B>
            & self.d;
}

struct X : C, D
{
    SERIALIZABLE(X)

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

        auto ar = oarchive(file);

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

        //
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

        auto ar = iarchive(file);

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
