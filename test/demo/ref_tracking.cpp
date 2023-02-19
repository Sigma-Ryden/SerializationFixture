#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Siraf/Core.hpp> // ReadArchive, WriteArchive
#include <Siraf/Memory/Ref.hpp>

using siraf::oarchive;
using siraf::iarchive;

using siraf::base;
using siraf::virtual_base;

using siraf::ref;

using siraf::memory::pure;

using namespace siraf::common; // support of common types
using namespace siraf::tracking; // support of data tracking

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

// Needed for clonable and serializable of derived object
struct B : POLYMORPHIC()
{
    SERIALIZABLE(B)

    int b = 0;
};
SERIALIZATION(SaveLoad, B)
{
    archive & self.b;
}

struct C : virtual B
{
    SERIALIZABLE(C)

    int c = 1;
};

SERIALIZATION(SaveLoad, C)
{
    archive & virtual_base<B>(self)
            & self.c;
}

struct D : virtual B
{
    SERIALIZABLE(D)

    int d = 22;
};

SERIALIZATION(SaveLoad, D)
{
    archive & virtual_base<B>(self) // expand to => virtual_base<B>
            & self.d;
}

struct X : C, D
{
    SERIALIZABLE(X)

    int x = 333;
};

SERIALIZATION(SaveLoad, X)
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

        auto ar = oarchive(file);

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

        auto ar = iarchive(file);

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
