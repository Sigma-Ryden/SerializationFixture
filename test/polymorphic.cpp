#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive, InnerRegistry

#include <Sifar/Support/string.hpp>

using sifar::reader;
using sifar::writer;

using sifar::InnerRegistry;

using namespace sifar::library; // support of std library
using namespace sifar::tracking; // support of data tracking

template <class SomeType>
class Base
{
    SERIALIZATION_ACCESS()
    SERIALIZATION_POLYMORPHIC_HASH(Base<SomeType>)

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
        ar & data;
    }

    SERIALIZATION_LOAD(ar)
    {
        std::cout << "Load Base class\n";
        ar & data;
    }
};
// partial class info
SERIALIZATION_POLYMORPHIC_TPL_HASH(Base<double>)
SERIALIZATION_POLYMORPHIC_TPL_HASH(Base<std::string>)

class Derived : public Base<std::string>
{
    SERIALIZATION_ACCESS()
    SERIALIZATION_POLYMORPHIC_HASH(Derived)

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
        sifar::base<Base>(ar, this);

        ar & value;
    }
};

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_polymorphic()
{
    using Registry = InnerRegistry<Base<std::string>, Derived>;

    using Parent = Base<std::string>;
    using Child  = Derived;

    {
        println(Registry::key<Base<char>>()); // <-- default
        println(Registry::key<Base<std::string>>()); // <-- specialization
    }
    //
    {
        std::ofstream file("test_polymorphic.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = writer<Registry>(file);

        Parent* b = new Parent("Hello!");
        Parent* d = new Child("Bye!", 3.1415926);

        ar & b;
        ar & d;

        println(Registry::key(*b));
        println(Registry::key(*d));

        b->show();
        d->show();

        delete b;
        delete d;

        file.close();
    }
    //
    //
    {
        std::ifstream file("test_polymorphic.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = reader<Registry>(file);

        Parent* b = nullptr;
        Parent* d = nullptr;

        ar & b;
        ar & d;

        println(Registry::key(*b));
        println(Registry::key(*d));

        b->show();
        d->show();

        delete b;
        delete d;

        file.close();
    }
    //
}

int main()
{
    test_polymorphic();

    return 0;
}
