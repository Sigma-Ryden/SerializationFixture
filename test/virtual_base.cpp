#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive, InnerRegistry

using sifar::reader;
using sifar::writer;

using sifar::base;
using sifar::virtual_base;

struct A : POLYMORPHIC_BASE()
{
    POLYMORPHIC(A)

    virtual ~A() {}
};

SERIALIZATION(A) {}

// virtual_base serialization proccess:
// if this->virtual_id != this->static_id
//     throw "the srializable object must serialize the virtual base object."
// else do something...
struct B : virtual A
{
    POLYMORPHIC(B)
};
SERIALIZATION(B)
{
    virtual_base<A>(archive, self); // works even for non-virtual base, but maybe ambiguous
}

struct C :  virtual A
{
    POLYMORPHIC(C)
};
SERIALIZATION(C)
{
    virtual_base<A>(archive, self);
}

struct D : B, C
{
    POLYMORPHIC(D)
};
SERIALIZATION(D)
{
    base<A>(archive, self);
    base<B>(archive, self);
    base<C>(archive, self);
}

struct F : D
{
    POLYMORPHIC(F)
};

SERIALIZATION(F)
{
    base<D>(archive, self);
}

void test_virtual_base()
{
    {
        std::ofstream file("test_virtual_base.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = writer(file);

        A* a = new F;
        std::cout << sifar::Access::dynamic_key(*a) << '\n';

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

        auto ar = reader(file);

        try
        {
            ar & a;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        std::cout << sifar::Access::dynamic_key(*a) << '\n';

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
