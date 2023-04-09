#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <SF/Core.hpp>// WriteArchive

#include <SF/Support/string.hpp>
#include <SF/Support/array.hpp>

using sf::WriteArchive;

// Impl of user wrapper for iostream
class FormatOutStream
{
private:
    std::ostream& stream_;

public:
    FormatOutStream(std::ostream& stream) : stream_(stream) {}

    template <typename T>
    void call(const T* data, std::size_t memory_size)
    {
        const std::size_t size = memory_size / sizeof(T);
        for (std::size_t i = 0; i < size; ++i)
            stream_ << data[i] << ' ';
    }

    void call(const char* data, std::size_t)
    {
        stream_ << data << ' ';
    }
};

using FormatWriteArchive = WriteArchive<std::ostream, FormatOutStream>;

enum Axis { X, Y, Z };
enum class Color { Red, Green, Blue, Alpha };

class B
{
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

public:
    friend std::ostream& operator<< (std::ostream& out, const B& obj)
    {
        return out << obj.pi_ << ' ' << *obj.data_;
    }
};

SERIALIZATION(Save, B)
{
    archive & self.pi_ & self.data_;
}

SERIALIZATION(Load, B)
{
    archive & self.pi_;
    archive & self.data_;
}

class A
{
    SERIALIZATION_ACCESS()

private:
    int x, y;
    B b;

public:
    A(int x = 0, int y = 0, float value = 0.f) : x(x), y(y), b(value) {}

public:
    friend std::ostream& operator<< (std::ostream& out, const A& obj)
    {
        return out << obj.x << ' ' << obj.y << ' ' << obj.b;
    }
};

SERIALIZATION(SaveLoad, A)
{
    archive & self.x & self.y & self.b;
}

// FormatWriteArchive overloading operator& for class B
auto operator& (FormatWriteArchive& ar, B& b) -> decltype(ar)
{
    std::cout << "overloading operator& for B";

    return ar;
}

void test_common()
{
    auto ar = sf::oarchive<FormatOutStream>(std::cout);

    constexpr auto xxx = sizeof(A);

    int number = 123;
    int* pointer = &number;

    std::string hi = "hello";
    char bye[] = "bye";

    //const char* boo = "boo"; // will serialize first symbol

    auto obj = new A(666, 999, 2.71);

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

    delete obj;
}

int main()
{
    test_common();

    return 0;
}
