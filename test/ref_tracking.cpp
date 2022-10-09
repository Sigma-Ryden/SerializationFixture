#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::writer;
using sifar::reader;

using sifar::base;
using sifar::virtual_base;

using sifar::ref;

using sifar::utility::pure;

using namespace sifar::common; // support of common types
using namespace sifar::tracking; // support of data tracking

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

// Needed for clonable and serializable of derived object
struct B : POLYMORPHIC_IMPORT()
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
    POLYMORPHIC()

    int c = 1;
};

SERIALIZATION_POLYMORPHIC(C)
{
    archive & virtual_base<B>(self)
            & self.c;
}

struct D : virtual B
{
    POLYMORPHIC()

    int d = 22;
};

SERIALIZATION_POLYMORPHIC(D)
{
    archive & virtual_base<B>(self) // expand to => virtual_base<B>
            & self.d;
}

struct X : C, D
{
    POLYMORPHIC()

    int x = 333;
};

SERIALIZATION_POLYMORPHIC(X)
{
    archive & virtual_base<B>(self)
            & base<C>(self)
            & base<D>(self)
            & self.x;
}
#define r(ref) &ref.get()

void test_ref_tracking()
{
    {
        std::ofstream file("test_tracking_virtual.bin", std::ios::binary);
        if (not file.is_open()) return;

        auto ar = writer(file);

        X x;
        auto d = ref<D>(x);
        auto c = ref<C>(x);
        auto b = ref<B>(x);

        println(&x);
        println(r(d));
        println(r(c));
        println(r(b));

        std::cout << '\n';

        println(pure(&x));
        println(pure(r(d)));
        println(pure(r(c)));
        println(pure(r(b)));

        try
        {
            ar & track(x);
            ar & d;
            ar & c;
            ar & b;
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

        X x;
        auto d = ref<D>();
        auto c = ref<C>();
        auto b = ref<B>();

        try
        {
            ar & track(x);
            ar & d;
            ar & c;
            ar & b;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        println(&x);
        println(r(d));
        println(r(c));
        println(r(b));

        std::cout << '\n';

        println(pure(&x));
        println(pure(r(d)));
        println(pure(r(c)));
        println(pure(r(b)));
    }
}

int main()
{
    test_ref_tracking();
    
    return 0;
}
