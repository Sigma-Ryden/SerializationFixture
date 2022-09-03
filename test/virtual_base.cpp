#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive, InnerRegistry

using sifar::reader;
using sifar::writer;

using sifar::InnerRegistry;

using sifar::base;
using sifar::virtual_base;

struct A
{
    virtual ~A() {}
    SERIALIZATION_POLYMORPHIC_KEY(0)

    SERIALIZATION_UNIFIED(ar) {}
};

// virtual_base serialization proccess:
// if this->virtual_id != this->static_id
//     throw "the srializable object must serialize the virtual base object."
// else do something...
struct B : virtual A
{
    SERIALIZATION_POLYMORPHIC_KEY(1)

    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<A>(ar, this); // works even for non-virtual base, but maybe ambiguous
    }
};

struct C :  virtual A
{
    SERIALIZATION_POLYMORPHIC_KEY(2)

    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<A>(ar, this);
    }
};

struct D : B, C
{
    SERIALIZATION_POLYMORPHIC_KEY(3)

    SERIALIZATION_UNIFIED(ar)
    {
        base<A>(ar, this);
        base<B>(ar, this);
        base<C>(ar, this);
    }
};

struct F : D
{
    SERIALIZATION_POLYMORPHIC_KEY(4)

    SERIALIZATION_UNIFIED(ar)
    {
        base<D>(ar, this);
    }
};

void test_virtual_base()
{
    // You also can use the 'SERIALIZATION_CLASS_EXPORT' for the 'sifar::ExternRegistry'
    using Registry = InnerRegistry<A, B, C, D, F>; // or just export A and F
    //
    {
        std::ofstream file("test_virtual_base.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = writer<Registry>(file);

        A* a = new F;
        std::cout << a->dynamic_key() << '\n';

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

        auto ar = reader<Registry>(file);

        try
        {
            ar & a;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        std::cout << a->dynamic_key() << '\n';

        file.close();

        delete a;
    }
    //
}
//
int main()
{
    test_virtual_base();

    return 0;
}
