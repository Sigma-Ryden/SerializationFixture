#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Siraf/Core.hpp> // ReadArchive, WriteArchive, InnerRegistry

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

// virtual_base serialization proccess:
// if this->virtual_id != this->static_id
//     throw "the srializable object must serialize the virtual base object."
// else do something...
struct B : virtual A
{
    SERIALIZABLE(B)
};
SERIALIZATION(SaveLoad, B)
{
    virtual_base<A>(archive, self); // works even for non-virtual base, but maybe ambiguous
}

struct C :  virtual A
{
    SERIALIZABLE(C)
};
SERIALIZATION(SaveLoad, C)
{
    virtual_base<A>(archive, self);
}

struct D : B, C
{
    SERIALIZABLE(D)
};
SERIALIZATION(SaveLoad, D)
{
    base<A>(archive, self);
    base<B>(archive, self);
    base<C>(archive, self);
}

struct F : D
{
    SERIALIZABLE(F)
};

SERIALIZATION(SaveLoad, F)
{
    base<D>(archive, self);
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
