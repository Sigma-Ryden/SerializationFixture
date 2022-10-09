#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive, InnerRegistry

#include <Sifar/Support/string.hpp>

using sifar::reader;
using sifar::writer;

using sifar::base;

using namespace sifar::library; // support of std library
using namespace sifar::tracking; // support of data tracking

template <class SomeType>
class Base : POLYMORPHIC_IMPORT()
{
    POLYMORPHIC_TEMPLATE(Base)

protected:
    SomeType data;

public:
    virtual ~Base() = default;

    Base(const SomeType& data = SomeType{}) : data(data) {}

    virtual void show()
    {
        std::cout << data << '\n';
    }
};
template <typename>
struct is_base : std::false_type {};

template <typename T>
struct is_base<Base<T>> : std::true_type {};

SERIALIZATION_SAVE(Base<std::string>)
{
    std::cout << "Save Base<std::string> class\n";
    archive & self.data;
}

SERIALIZATION_CONDITIONAL_SAVE(is_base<T>::value)
{
    std::cout << "Load Base class\n";
    archive & self.data;
}

SERIALIZATION_CONDITIONAL_LOAD(is_base<T>::value)
{
    std::cout << "Load Base class\n";
    archive & self.data;
}

class Derived : public Base<std::string>
{
    POLYMORPHIC()

private:
    float value;

public:
    Derived(std::string data = "", float c = 0.) : Base(data), value(c) {}

    virtual void show() override
    {
        std::cout << data << ' ' << value << '\n';
    }

private:
};

SERIALIZATION_SAVE_LOAD(Derived)
{
    std::cout << "Save Load Derived class\n";
    archive & base<Base<std::string>>(self);
    archive & self.value;
}

POLYMORPHIC_TEMPLATE_EXPORT(Base<std::string>)
POLYMORPHIC_TEMPLATE_EXPORT(Base<double>)

POLYMORPHIC_EXPORT(Derived)

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_polymorphic()
{
    using Registry = sifar::dynamic::ExternRegistry;

    using Parent = Base<std::string>;
    using Child  = Derived;

    {
        println(Registry::key<Base<char>>()); // <-- default value
        println(Registry::key<Base<std::string>>()); // <-- specialization
        println(Registry::key<Base<double>>()); // <-- specialization
    }
    //
    {
        std::ofstream file("test_polymorphic.bin", std::ios::binary);

        if (not file.is_open()) return;

        auto ar = writer(file);

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

        auto ar = reader(file);

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
