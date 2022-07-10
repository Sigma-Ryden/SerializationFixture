#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp>// WriteArchive, ExternRegistry

#include <Sifar/Support/string.hpp>
#include <Sifar/Support/array.hpp>

using sifar::WriteArchive;
using sifar::ExternRegistry;

using namespace sifar::library;

// Impl of user wrapper for iostream
class FormatOutStream
{
private:
    std::ostream& stream_;

public:
    FormatOutStream(std::ostream& stream) : stream_(stream) {}

    template <typename T>
    FormatOutStream& write(const T* data, std::size_t)
    {
        stream_ << *data << ' ';

        return *this;
    }
};

using FormatWriteArchive = WriteArchive<std::ostream, ExternRegistry, FormatOutStream>;

enum Axis { X, Y, Z };
enum class Color { Red, Green, Blue, Alpha };

class B
{
    // is the same: friend class serialization::Access
    SERIALIZATION_ACCESS()

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
    SERIALIZATION_SAVE(ar)
    {
        ar & pi_ & data_;
    }

    SERIALIZATION_LOAD(ar)
    {
        ar & pi_;
        ar & data_;
    }

public:
    friend std::ostream& operator<< (std::ostream& out, const B& obj)
    {
        return out << obj.pi_ << ' ' << *obj.data_;
    }
};

class A
{
    // not required for public save/load function
    //SERIALIZATION_ACCESS()

private:
    int x, y;
    B b;

public:
    A(int x = 0, int y = 0, float value = 0.f) : x(x), y(y), b(value) {}

public:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & x & y & b;
    }

    friend std::ostream& operator<< (std::ostream& out, const A& obj)
    {
        return out << obj.x << ' ' << obj.y << ' ' << obj.b;
    }
};

// FormatWriteArchive overloading operator& for class B
auto operator& (FormatWriteArchive& archive, B& t) -> decltype(archive)
{
    std::cout << "overloading operator& for B";

    return archive;
}

void test_common()
{
    FormatWriteArchive ar(std::cout);

    int number = 123;
    int* pointer = &number;

    std::string hi = "hello";
    char bye[] = "bye";

    //const char* boo = "boo"; // will serialize first symbol

    A obj(666, 999, 2.71);

    float array[] { 1.1, 2.2 , 3.3 };
    int matrix[4][3] { { }, { 1 }, { 2, 3 }, { 4, 5, 6 } };

    std::array<int, 6> std_array { 123, 132, 213, 231, 312, 321 };

    auto axis = Axis::X; // classic C enum
    auto color = Color::Blue; // scoped enum

    ar & hi;
    ar & number;

    ar & pointer;
    ar & array;
    ar & matrix;
    ar & obj;

    std::cout << '\n';

    ar << axis;
    ar << color;
    ar << bye;

    ar & std_array;

    std::cout << '\n';
}

int main()
{
    test_common();

    return 0;
}
