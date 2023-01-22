#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#define SIRAF_DEBUG
#include <Siraf/Core.hpp> // ReadArchive, WriteArchive

#include <Siraf/Support/string.hpp>

using siraf::iarchive;
using siraf::oarchive;

using siraf::base;

using namespace siraf::library; // support of std library

template <class SomeType>
class Base : POLYMORPHIC()
{
    SERIALIZABLE(Base)

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

template <typename> struct is_base : std::false_type {};
template <typename T> struct is_base<Base<T>> : std::true_type {};

SERIALIZATION(Save, Base<std::string>)
{
    std::cout << "Save Base<std::string> class\n";
    archive & self.data;
}

CONDITIONAL_SERIALIZATION(Save, is_base<T>::value)
{
    std::cout << "Save Base class\n";
    archive & self.data;
}

CONDITIONAL_SERIALIZATION(Load, is_base<T>::value)
{
    std::cout << "Load Base class\n";
    archive & self.data;
}

namespace internal // example namespace
{

class Derived : public Base<std::string>
{
    SERIALIZABLE(internal::Derived)

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

SERIALIZATION(SaveLoad, internal::Derived)
{
    std::cout << "Save/Load Derived class\n";
    archive & base<Base<std::string>>(self);
    archive & self.value;
}

// EXPORT_POLYMORPHIC(internal::Derived) // - not required
EXPORT_POLYMORPHIC(Base<std::string>)
EXPORT_POLYMORPHIC(Base<double>) // smae as EXPORT_POLYMORPHIC_KEY("Base<double>", Base<double>)

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

void test_polymorphic()
{
    using Registry = siraf::dynamic::RegistryBase;

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

        auto ar = oarchive(file);

        Parent* b = nullptr;
        Parent* d = nullptr;

        try
        {
            b = new Parent("Hello!");
            d = new Child("Bye!", 3.1415926);

            ar & b;
            ar & d;
        }
        catch (const char* e)
        {
            println(e);

            file.close();
            return;
        }

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

        auto ar = iarchive(file);

        Parent* b = nullptr;
        Parent* d = nullptr;

        try
        {
            ar & b;
            ar & d;
        }
        catch (const char* e)
        {
            println(e);

            file.close();
            return;
        }

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
   return 0;
}
