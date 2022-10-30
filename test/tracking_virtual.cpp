#include <iostream> // cout
#include <fstream> // ifstream, ofstream

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::reader;
using sifar::writer;

using sifar::virtual_base;
using sifar::hierarchy;
using sifar::native_base;

using sifar::utility::pure;

using namespace sifar::common; // support of common types
using namespace sifar::tracking; // support of data tracking

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

// Needed for clonable and serializable of derived object
struct B : IMPORT_POLYMORPHIC() // same as IMPORT_SERIALIZABLE_POLYMORPHIC
{
    POLYMORPHIC()

    int b = 0;
};
SERIALIZATION_POLYMORPHIC(B)
{
    archive & self.b;
}

struct C : virtual B
{
    SERIALIZABLE_POLYMORPHIC()

    int c = 1;
};
EXPORT_SERIALIZABLE_POLYMORPHIC(C)
SAVE_LOAD_SERIALIZABLE(C)
{
    archive & virtual_base<B>(self)
            & self.c;
}

struct D : virtual B
{
    POLYMORPHIC() // same as SERIALIZABLE_POLYMORPHIC()

    int d = 22;
};

SERIALIZATION_POLYMORPHIC(D) // same as: EXPORT_POLYMORPHIC(D) SAVE_LOAD_SERIALIZABLE(D) {...}
{
    archive & native_base<B>(self) // expand to => virtual_base<B>
            & self.d;
}

struct X : C, D
{
    POLYMORPHIC()

    int x = 333;
};

SERIALIZATION_POLYMORPHIC(X)
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
            ar & track(d)
               & track(b)
               & track(c)
               & track(x);
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
            ar & track(c)
               & track(d)
               & track(x)
               & track(b);
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
