#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Siraf/Core.hpp> // ReadArchive, WriteArchive

using siraf::iarchive;
using siraf::oarchive;

using siraf::base;
using siraf::virtual_base;

struct A : POLYMORPHIC()
{
    SERIALIZABLE(A)

    virtual ~A() {}
};

SERIALIZATION(SaveLoad, A) {}

struct B : virtual A
{
    SERIALIZABLE(B)
};
SERIALIZATION(SaveLoad, B)
{
    archive & virtual_base<A>(self); // works even for non-virtual base, but maybe ambiguous
}

struct C : virtual A
{
    SERIALIZABLE(C)
};
SERIALIZATION(SaveLoad, C)
{
    virtual_base<A>(archive, self); // same as archive & virtual_base<A>(self);
}

struct D : B, C
{
    SERIALIZABLE(D)
};
SERIALIZATION(SaveLoad, D)
{
    archive & base<A>(self)
            & base<B>(self)
            & base<C>(self); // or archive & hierarchy<A, B, C>(self);
}

struct F : D
{
    SERIALIZABLE(F)
};

SERIALIZATION(SaveLoad, F)
{
    base<D>(archive, self); // same as archive & base<D>(self);
}

void test_virtual_base()
{
    {
        std::ofstream file("test_virtual_base.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = oarchive(file);

        A* a = new F;
        std::cout << siraf::Access::trait(*a) << '\n';

        try
        {
            ar & a;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        file.close();

        delete a;
    }
    //
    //
    {
        A* a = nullptr;

        std::ifstream file("test_virtual_base.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = iarchive(file);

        try
        {
            ar & a;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        std::cout << siraf::Access::trait(*a) << '\n';

        file.close();

        delete a;
    }
    //
}

int main()
{
    test_virtual_base();

    return 0;
}
