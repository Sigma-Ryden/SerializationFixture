#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadArchive, WriteArchive, InnerRegistry

#include <Sifar/Support/string.hpp>

using sifar::reader;
using sifar::writer;

using sifar::base;

using namespace sifar::library; // support of std library

template <class SomeType>
class Base : POLYMORPHIC_BASE()
{
    POLYMORPHIC(Base)

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

SAVE_LOAD_SERIALIZABLE(Base<std::string>)
{
    std::cout << "Save Base<std::string> class\n";
    archive & self.data;
}

CONDITIONAL_SAVE_SERIALIZABLE(is_base<T>::value)
{
    std::cout << "Load Base class\n";
    archive & self.data;
}

CONDITIONAL_LOAD_SERIALIZABLE(is_base<T>::value)
{
    std::cout << "Load Base class\n";
    archive & self.data;
}

namespace internal // example namespace
{

class Derived : public Base<std::string>
{
    POLYMORPHIC(internal::Derived)

private:
    float value;

public:
    Derived(std::string data = "", float c = 0.) : Base(data), value(c) {}

    virtual void show() override
    {
        std::cout << data << ' ' << value << '\n';
    }
};

} // namespace internal

SAVE_LOAD_SERIALIZABLE(internal::Derived)
{
    std::cout << "Save/Load Derived class\n";
    archive & base<Base<std::string>>(self);
    archive & self.value;
}

// EXPORT_POLYMORPHIC(internal::Derived)// - not required
EXPORT_POLYMORPHIC(Base<std::string>)
EXPORT_POLYMORPHIC(Base<double>) // smae as EXPORT_POLYMORPHIC_KEY("Base<double>", Base<double>)

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_polymorphic()
{
    using Registry = sifar::dynamic::ExternRegistry;

    using Parent = Base<std::string>;
    using Child  = internal::Derived;
    {
        println(Registry::key<Base<char>>()); // <-- default value
        println(Registry::key<Base<std::string>>()); // <-- exported key
        println(Registry::key<Base<double>>()); // <-- exported key
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

        // for shared: if exist in raw then add to shared (if can) and serialize key only
        // for raw: if exist in shared then add to raw (if can) and serialize key only
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
