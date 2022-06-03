#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <Serialization/Core.hpp>
#include <Serialization/Support/string.hpp>

namespace sr = serialization;

using namespace sr::common; // support of common types
using namespace sr::library; // support of std library
using namespace sr::tracking; // support of data tracking

template <class SomeType>
struct Base
{
    SERIALIZATION_ARCHIVE_ACCESS()
    SERIALIZATION_IMPLEMENT_CLASS_INFO(Base<SomeType>)

protected:
    SomeType data;

public:
    virtual ~Base() = default;

    Base(const SomeType& data = "") : data(data) {}

    virtual void show()
    {
        std::cout << data << '\n';
    }

private:
    SERIALIZATION_SAVE(ar)
    {
        std::cout << "Save Base class\n";
        return ar & data;
    }

    SERIALIZATION_LOAD(ar)
    {
        std::cout << "Load Base class\n";
        return ar & data;
    }
};
// partial class info
SERIALIZATION_IMPLEMENT_CLASS_TPL_INFO(Base<double>)
SERIALIZATION_IMPLEMENT_CLASS_TPL_INFO(Base<std::string>)

struct Derived : Base<std::string>
{
    SERIALIZATION_ARCHIVE_ACCESS()
    SERIALIZATION_IMPLEMENT_CLASS_INFO(Derived)

private:
    float value;

public:
    Derived(std::string data = "", float c = 0.) : Base(data), value(c) {}

    virtual void show()
    {
        std::cout << data << ' ' << value << '\n';
    }

private:
    SERIALIZATION_UNIFIED(ar)
    {
        sr::base<Base>(ar, *this);

        ar & value;

        return ar;
    }
};

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_polymorphic()
{
    using Registry = sr::Registry<Base<std::string>, Derived>;

    using WriteArchive = sr::WriteArchive<std::ofstream, Registry>;
    using ReadArchive  = sr::ReadArchive<std::ifstream, Registry>;

    using Parent = Base<std::string>;
    using Child  = Derived;

    {
        println(Base<char>::static_index()); // <-- default
        println(Base<std::string>::static_index()); // <-- specialization
    }
    //
    {
        std::ofstream file("D:/test.bin", std::ios::binary);

        if (not file.is_open()) return;

        WriteArchive ar(file);

        Parent* b = new Parent("Hello!");
        Parent* d = new Child("Bye!", 3.1415926);

        ar & b;
        ar & d;

        println(b->index());
        println(d->index());

        b->show();
        d->show();

        delete b;
        delete d;

        file.close();
    }
    //
    //
    {
        std::ifstream file("D:/test.bin", std::ios::binary);

        if (not file.is_open()) return;

        ReadArchive ar(file);

        Parent* b = nullptr;
        Parent* d = nullptr;

        ar & b;
        ar & d;

        println(b->index());
        println(d->index());

        b->show();
        d->show();

        delete b;
        delete d;

        file.close();
    }
    //
}


struct A
{
    virtual ~A() {}

    static constexpr int static_index() { return 0; }
    virtual int index() { return 0; }

    SERIALIZATION_UNIFIED(ar) { return ar; }
};

// virtual_base serialization proccess:
// if this->virtual_id != this->static_id
//     throw "the srializable object must serialize the virtual base object."
// else do something...
struct B : virtual A
{
    virtual int index() { return 1; }
    static constexpr int static_index() { return 1; }

    SERIALIZATION_UNIFIED(ar)
    {
        sr::virtual_base<A>(ar, *this);
        return ar;
    }
};

struct C :  virtual A
{
    virtual int index() { return 2; }
    static constexpr int static_index() { return 2; }

    SERIALIZATION_UNIFIED(ar)
    {
        sr::virtual_base<A>(ar, *this);

        return ar;
    }
};

struct D : B, C
{
    virtual int index() { return 3; }
    static constexpr int static_index() { return 3; }

    SERIALIZATION_UNIFIED(ar)
    {
        sr::base<A>(ar, *this);
        sr::base<B>(ar, *this);
        sr::base<C>(ar, *this);

        return ar;
    }
};
struct F : D
{
    virtual int index() { return 4; }
    static constexpr int static_index() { return 4; }

    SERIALIZATION_UNIFIED(ar)
    {
        serialization::base<D>(ar, *this);

        return ar;
    }
};

void test_virtual_base()
{
    using Registry = sr::Registry<A, B, C, D, F>;

    using WriteArchive = sr::WriteArchive<std::ofstream, Registry>;
    using ReadArchive  = sr::ReadArchive<std::ifstream, Registry>;
    //
    {
        A* a = new F;
        std::cout << a->index() << '\n';

        std::ofstream file("D:/test.bin", std::ios::binary);

        if (not file.is_open()) return;

        WriteArchive ar(file);

        try
        {
            ar & a;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
        }

        file.close();

        delete a;
    }
    //
    //
    {
        A* a = nullptr;

        std::ifstream file("D:/test.bin", std::ios::binary);

        if (not file.is_open()) return;

        ReadArchive ar(file);

        try
        {
            ar & a;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
        }

        std::cout << a->index() << '\n';

        file.close();

        delete a;
    }
    //
}

int main()
{
    test_polymorphic();
    test_virtual_base();

    return 0;
}
