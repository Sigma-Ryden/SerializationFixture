#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include "Serialization/Core.hpp"

namespace sr = serialization;

using namespace sr::common;

struct Base
{
    SERIALIZATION_ARCHIVE_ACCESS()
    SERIALIZATION_IMPLEMENT_CLASS_INFO(0)

protected:
    int x;

public:
    virtual ~Base() = default;

    Base(int x = 0) : x(x) {}

    virtual void show()
    {
        std::cout << x << '\n';
    }

private:
    SERIALIZATION_SAVE(ar)
    {
        std::cout << "Save Base class\n";
        return ar & x;
    }

    SERIALIZATION_LOAD(ar)
    {
        std::cout << "Load Base class\n";
        return ar & x;
    }
};

struct Derived : Base
{
    SERIALIZATION_ARCHIVE_ACCESS()
    SERIALIZATION_IMPLEMENT_CLASS_INFO(1)

private:
    float y;

public:
    Derived(int x = 0, float y = 0.) : Base(x), y(y) {}

    virtual void show()
    {
        std::cout << x << ' ' << y << '\n';
    }

private:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & sr::base<Base>(*this);
        ar & y;

        return ar;
    }
};

#define println(...) \
    std::cout << (#__VA_ARGS__) << " : " << (__VA_ARGS__) << '\n'

int main()
{
    using Registry = sr::Registry<Base, Derived>;

    using WriteArchive = sr::WriteArchive<Registry, std::ofstream>;
    using ReadArchive = sr::ReadArchive<Registry, std::ifstream>;
    //
    {
        std::ofstream file("D:/test.bin", std::ios::binary);

        WriteArchive ar(file);

        Base* b = new Base(777);
        Base* d = new Derived(1010101, 3.1415926);

        ar & b;
        ar & d;

        println(b->index());
        println(d->index());

        b->show();
        d->show();

        delete b;
        delete d;
    }
    //
    //
    {
        std::ifstream file("D:/test.bin", std::ios::binary);

        ReadArchive ar(file);

        Base* b = nullptr;
        Base* d = nullptr;

        ar & b;
        ar & d;

        println(b->index());
        println(d->index());

        b->show();
        d->show();

        delete b;
        delete d;
    }
    //
    return 0;
}
/*
#include <array> // array
#include <vector> // vector
#include <string> // string
*/
// support library
/*
#include "Serialization/Support/string.hpp"
#include "Serialization/Support/vector.hpp"
*/
/*
using namespace sr::library;
*/
/*
template <typename InStream>
class FormatInStreamWrapper
{
private:
    InStream& stream_;

public:
    FormatInStreamWrapper(InStream& stream) : stream_(stream) {}

    template <typename T>
    FormatInStreamWrapper& read(T& data, std::size_t)
    {
        stream_ >> data;

        return *this;
    }
};

template <typename OutStream>
class FormatOutStreamWrapper
{
private:
    OutStream& stream_;

public:
    FormatOutStreamWrapper(OutStream& stream) : stream_(stream) {}

    template <typename T>
    FormatOutStreamWrapper& write(const T& data, std::size_t)
    {
        stream_ << data << ' ';

        return *this;
    }
};
*/
/*
class B
{
    // is same friend class serialization::Access;
    SERIALIZATION_ARCHIVE_ACCESS()

private:
    float pi_;
    float* data_;

public:
    B(float value = 0) : pi_(value)
    {
        data_ = new float(2. + value);
    }

    ~B() { delete data_; }

private:
    template <class Archive>
    Archive& save(Archive& ar)
    {
        return ar & pi_ & data_;
    }

    template <class Archive>
    Archive& load(Archive& ar)
    {
        ar & pi_;
        ar & data_;

        return ar;
    }

public:
    friend std::ostream& operator<< (std::ostream& out, const B& obj)
    {
        return out << obj.pi_ << ' ' << *obj.data_;
    }
};

class A
{
    // not required for public serialize or save/load function
    //SERIALIZATION_ARCHIVE_ACCESS()

private:
    int x, y;
    B b;

public:
    A(int x = 0, int y = 0, float value = 0.f) : x(x), y(y), b(value) {}

public:
    template <class Archive>
    Archive& serialize(Archive& ar)
    {
        return ar & x & y & b;
    }

    friend std::ostream& operator<< (std::ostream& out, const A& obj)
    {
        return out << obj.x << ' ' << obj.y << ' ' << obj.b;
    }
};

enum Axis { X, Y, Z };
enum class Color { Red, Green, Blue, Alpha };
*/
/*
template <typename OutStream>
using FormatWriteArchive = sr::WriteArchive<OutStream, FormatOutStreamWrapper<OutStream>>;

// FormatWriteArchive overloading operator& for class B
template <typename OutStream>
auto operator& (FormatWriteArchive<OutStream>& archive, B& t) -> decltype(archive)
{
    std::cout << "overloading operator& for B";

    return archive;
}

void test_std_array_serialization()
{
    FormatWriteArchive<std::ostream> ar(std::cout);

    std::array<int, 6> std_array { 123, 132, 213, 231, 312, 321 };

    ar & std_array;
}

void test_more()
{
    FormatWriteArchive<std::ostream> ar(std::cout);

    int number = 123;
    int* pointer = &number;

    std::string hi = "hello";
    char bye[] = "bye";

    const char* boo = "boo";

    A obj(666, 999, 2.71);

    float array[] { 1.1, 2.2 , 3.3 };
    int matrix[4][3] { { }, { 1 }, { 2, 3 }, { 4, 5, 6 } };

    auto axis = Axis::X; // classic C enum
    auto color = Color::Blue; // scoped enum

    const int height = 3;
    const int width = 4;

    int* dynamic_array = new int [width] { -1, -2, -3, -4 };

    int** dynamic_tensor = new int* [height];
    for (int i = 0; i < width; ++i)
        dynamic_tensor[i] = new int [width] { i, i + 1, i, i - 1 };

    auto scope_boo = sr::make::scope(boo);
    auto scope_tensor = sr::make::scope(dynamic_tensor, sr::make::dim(height, width));
    auto scope_array = sr::make::scope(dynamic_array, width);

    std::ofstream file("D:/x.bin");
    auto ar2 = sr::make::write_archive(file);
    ar2 & hi;
    ar & hi;
    ar & number;

    ar & pointer;
    ar & array;
    ar & matrix;
    ar & obj;
    ar & scope_array;

    ar << scope_tensor;
    ar << axis;
    ar << scope_boo;
    ar << color;
    ar << bye;
}

void test_object_serialization()
{
    {
        std::ofstream file("D:\\experimental.bin");

        if (not file.is_open()) return;

        auto ar = sr::make::write_archive(file);

        A obj(1, 2, 3.1415926f);

        std::string hi = "Hello!";
        char bye[] = "Good Bye!";

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Serialization done.\n";
    }

    {
        std::ifstream file("D:\\experimental.bin");

        if (not file.is_open()) return;

        auto ar = sr::make::read_archive(file);

        A obj;

        std::string hi = "Hello!";
        char bye[10];

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Deserialization done.\n";

        std::cout << obj << ' ' << hi << ' ' << bye << '\n';
    }
}

class Human
{
    friend class sr::Access;

public:
//private:
    std::string name_;
    int age_;

public:
    Human(const std::string& name, int age)
        : name_(name), age_(age)
    {
    }

private:
    template <class Archive>
    Archive& serialize(Archive& ar)
    {
        ar & name_;
        ar & age_;

        return ar;
    }
};

class Boy : public Human
{
    friend class sr::Access;

public:
//private:
    int force_;

public:
    Boy() : Human("untitled", 0), force_(0)
    {
    }

    Boy(const std::string& name, int age, int force)
        : Human(name, age), force_(force)
    {
    }

private:
    template <class Archive>
    Archive& serialize(Archive& ar)
    {
        ar & sr::base<Human>(*this);

        ar & force_;

        return ar;
    }
};

class Animal
{
    friend class sr::Access;

public:
    using size_type = std::size_t;

protected:
    std::string m_name;
    std::string m_speak;

public:
    Animal(std::string name = "...", std::string speak = "???") : m_name(name), m_speak(speak)
    {
    }

public:
    const std::string& getName() const { return m_name; }
    const std::string& speak() const { return m_speak; }

private:
    template <class Archive>
    Archive& serialize(Archive& ar)
    {
        ar & m_name;
        ar & m_speak;

        return ar;
    }
};

class Cat : public Animal
{
    friend class sr::Access;

public:
    Cat(std::string name)
        : Animal(name, "Meow")
    {
    }

    const char* speak() { return "Meow"; }

private:
    template <class Archive>
    Archive& serialize(Archive& ar)
    {
        ar & sr::base<Animal>(*this);

        return ar;
    }
};

class Dog : public Animal
{
    friend class sr::Access;

public:
    Dog(std::string name)
        : Animal(name, "Woof")
    {
    }

    const char* speak() { return "Woof"; }

private:
    template <class Archive>
    Archive& serialize(Archive& ar)
    {
        ar & sr::base<Animal>(*this);

        return ar;
    }
};

void test_derived()
{
    using sr::detail::size;

    {
        std::vector<Animal*> zoo;
        zoo.reserve(3);

        zoo.push_back(new Cat("Matros"));
        zoo.push_back(new Dog("Barsik"));
        zoo.push_back(new Cat("Ivan"));

        for (const auto& animal : zoo)
            std::cout << animal->getName() << " says " << animal->speak() << '\n';

        std::ofstream file("D:\\experimental.bin");

        if (not file.is_open()) return;

        auto ar = sr::make::write_archive(file);

        ar & zoo;

        file.close();

        std::cout << "Serialization done.\n";
    }

    {
        std::vector<Animal*> zoo;

        std::ifstream file("D:\\experimental.bin");

        if (not file.is_open()) return;

        auto ar = sr::make::read_archive(file);

        ar & zoo;

        file.close();

        std::cout << "Deserialization done.\n";

        for (const auto& animal : zoo)
            std::cout << animal->getName() << " says " << animal->speak() << '\n';
    }
}
*/
