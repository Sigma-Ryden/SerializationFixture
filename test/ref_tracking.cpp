#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive

using sifar::ReadArchive;
using sifar::WriteArchive;

using sifar::base;
using sifar::virtual_base;

using sifar::utility::pure;

using namespace sifar::common; // support of common types
using namespace sifar::tracking; // support of data tracking

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

struct B
{
    int b = 0;
    virtual ~B() = default;

    SERIALIZATION_POLYMORPHIC_KEY(0)
    SERIALIZATION_UNIFIED(ar)
    {
        ar & b;
    }
};

struct C : virtual B
{
    int c = 1;
    SERIALIZATION_POLYMORPHIC_KEY(1)
    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<B>(ar, *this);
        ar & c;
    }
};

struct D : virtual B
{
    int d = 22;
    SERIALIZATION_POLYMORPHIC_KEY(2)
    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<B>(ar, *this);
        ar & d;
    }
};

struct X : C, D
{
    int x = 333;
    SERIALIZATION_POLYMORPHIC_KEY(3)
    SERIALIZATION_UNIFIED(ar)
    {
        virtual_base<B>(ar, *this);
        base<C>(ar, *this);
        base<D>(ar, *this);

        ar & x;
    }
};

SERIALIZATION_EXPORT_KEY(B)
SERIALIZATION_EXPORT_KEY(C)
SERIALIZATION_EXPORT_KEY(D)
SERIALIZATION_EXPORT_KEY(X)

#define _(xx) &xx.get()

void test_ref_tracking()
{
    using sifar::utility::Ref;
    {
        std::ofstream file("test_tracking_virtual.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = WriteArchive<std::ofstream>(file);

        X x;
    
        Ref<D> d = x;
        Ref<C> c = x;
        Ref<B> b = x;

        println(&x);
        println(_(d));
        println(_(c));
        println(_(b));

        std::cout << '\n';

        println(pure(&x));
        println(pure(_(d)));
        println(pure(_(c)));
        println(pure(_(b)));

        try
        {
            track(ar, x);
            ar(d, c, b);
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

        auto ar = ReadArchive<std::ifstream>(file);

        X x;
    
        Ref<D> d;
        Ref<C> c;
        Ref<B> b;

        try
        {
            track(ar, x);
            ar(d, c, b);
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        println(&x);
        println(_(d));
        println(_(c));
        println(_(b));

        std::cout << '\n';

        println(pure(&x));
        println(pure(_(d)));
        println(pure(_(c)));
        println(pure(_(b)));
    }
}

int main()
{
    test_ref_tracking();
    
    
    return 0;
}