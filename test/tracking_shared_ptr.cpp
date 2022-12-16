#include <iostream> // cout
#include <fstream> // ifstream, ofstream

#define SIFAR_DEBUG
#include <Siraf/Core.hpp> // ReadArchive, WriteArchive

#include <Siraf/Support/shared_ptr.hpp>

using siraf::iarchive;
using siraf::oarchive;

using siraf::hierarchy;

using siraf::memory::pure;

using namespace siraf::common; // support of common types
using namespace siraf::tracking; // support of data tracking

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

struct B : POLYMORPHIC_BASE()
{
    POLYMORPHIC(B)

    int b = 0;
};

struct C : virtual B
{
    POLYMORPHIC(C)

    int c = 1;
};

struct D : virtual B
{
    POLYMORPHIC(D)

    int d = 22;
};

struct X : C, D
{
    POLYMORPHIC(X)

    int x = 333;
};

SAVE_LOAD_SERIALIZABLE(B)
{
    archive & self.b;
}

SAVE_LOAD_SERIALIZABLE(C)
{
    archive & hierarchy<B>(self)
            & self.c;
}

SAVE_LOAD_SERIALIZABLE(D)
{
    archive & hierarchy<B>(self)
            & self.d;
}

SAVE_LOAD_SERIALIZABLE(X)
{
    archive & hierarchy<B, C, D>(self)
            & self.x;
}

void test_tracking_shared_ptr()
{
    {
        std::ofstream file("test_tracking_virtual.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = oarchive(file);

        auto xxx = siraf::Access::static_key<X>();

        std::shared_ptr<X> x = std::make_shared<X>();
        std::shared_ptr<D> d = x;
        std::shared_ptr<C> c = x;
        std::shared_ptr<B> b = x;

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

        auto ar = iarchive(file);

        std::shared_ptr<X> x = nullptr;
        std::shared_ptr<D> d = nullptr;
        std::shared_ptr<C> c = nullptr;
        std::shared_ptr<B> b = nullptr;

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
    test_tracking_shared_ptr();

    return 0;
}
