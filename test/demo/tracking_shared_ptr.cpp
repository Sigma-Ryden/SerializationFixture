#include <iostream> // cout
#include <fstream> // ifstream, ofstream

#define SIRAF_DEBUG
#include <Siraf/Core.hpp> // ReadArchive, WriteArchive

#include <Siraf/Support/shared_ptr.hpp>

using siraf::iarchive;
using siraf::oarchive;

using siraf::hierarchy;

using siraf::memory::pure;

#define println(expr) std::cout << '\t' << #expr << " : " << expr << '\n';

struct B : POLYMORPHIC()
{
    SERIALIZABLE(B)

    int b = 0;
};

struct C : virtual B
{
    SERIALIZABLE(C)

    int c = 1;
};

struct D : virtual B
{
    SERIALIZABLE(D)

    int d = 22;
};

struct X : C, D
{
    SERIALIZABLE(X)

    int x = 333;
};

SERIALIZATION(SaveLoad, B)
{
    archive & self.b;
}

SERIALIZATION(SaveLoad, C)
{
    archive & hierarchy<B>(self)
            & self.c;
}

SERIALIZATION(SaveLoad, D)
{
    archive & hierarchy<B>(self)
            & self.d;
}

SERIALIZATION(SaveLoad, X)
{
    archive & hierarchy<B, C, D>(self)
            & self.x;
}

void test_tracking_shared_ptr()
{
    siraf::ByteContainer storage;
    {
        auto ar = oarchive(storage);

        auto xxx = siraf::Access::static_trait<X>();

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
        auto ar = iarchive(storage);

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
