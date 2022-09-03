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

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

#define println_ref(ref) \
    std::cout << (#ref) << " : " << (&ref.get()) << '\n'

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
        ar & virtual_base<B>(this);
        ar & c;
    }
};

struct D : virtual B
{
    int d = 22;
    SERIALIZATION_POLYMORPHIC_KEY(2)
    SERIALIZATION_UNIFIED(ar)
    {
        ar & virtual_base<B>(this);
        ar & d;
    }
};

struct X : C, D
{
    int x = 333;
    SERIALIZATION_POLYMORPHIC_KEY(3)
    SERIALIZATION_UNIFIED(ar)
    {
        ar & virtual_base<B>(this);
        ar & base<C>(this);
        ar & base<D>(this);

        ar & x;
    }
};

SERIALIZATION_EXPORT_KEY(B)
SERIALIZATION_EXPORT_KEY(C)
SERIALIZATION_EXPORT_KEY(D)
SERIALIZATION_EXPORT_KEY(X)

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
