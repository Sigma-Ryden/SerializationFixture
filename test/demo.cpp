#include <array> // array
#include <vector> // vector
#include <string> // string

#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Serialization/Core.hpp>

#include <Serialization/Support/string.hpp>
#include <Serialization/Support/array.hpp>

namespace sr = serialization;

using namespace sr::library;

// Impl of user wrapper for iostream
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

class B
{
    // is the same: friend class serialization::Access
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
    SERIALIZATION_SAVE(ar)
    {
        return ar & pi_ & data_;
    }

    SERIALIZATION_LOAD(ar)
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
    // not required for public save/load function
    //SERIALIZATION_ARCHIVE_ACCESS()

private:
    int x, y;
    B b;

public:
    A(int x = 0, int y = 0, float value = 0.f) : x(x), y(y), b(value) {}

public:
    SERIALIZATION_UNIFIED(ar)
    {
        return ar & x & y & b;
    }

    friend std::ostream& operator<< (std::ostream& out, const A& obj)
    {
        return out << obj.x << ' ' << obj.y << ' ' << obj.b;
    }
};

class Human
{
    SERIALIZATION_ARCHIVE_ACCESS()

protected:
    std::string name_;
    int age_;

public:
    Human(const std::string& name, int age)
        : name_(name), age_(age)
    {
    }

    const std::string& name() const noexcept { return name_; }
    int age() const noexcept { return age_; }

private:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & name_;
        ar & age_;

        return ar;
    }
};

class Boy : public Human
{
private:
    int force_;

public:
    Boy() : Human("unnamed", 0), force_(0)
    {
    }

    Boy(const std::string& name, int age, int force)
        : Human(name, age), force_(force)
    {
    }

    int force() const noexcept { return force_; }

    SERIALIZATION_UNIFIED(ar)
    {
        serialization::base<Human>(ar, *this);

        ar & force_;

        return ar;
    }
};

enum Axis { X, Y, Z };
enum class Color { Red, Green, Blue, Alpha };

template <typename OutStream>
using FormatWriteArchive =
    sr::WriteArchive<OutStream, sr::Registry<>, FormatOutStreamWrapper<OutStream>>;

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

void test_common()
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

    std::cout << '\n';
}

void test_object_serialization()
{
    {
        std::ofstream file("D:\\test_object_serialization.bin");

        if (not file.is_open()) return;

        auto ar = sr::WriteArchive<std::ofstream>(file);

        Boy obj("Tom", 21, 9);

        std::string hi = "Hello!";
        char bye[] = "Good Bye!";

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Serialization done.\n";
    }
    {
        std::ifstream file("D:\\test_object_serialization.bin");

        if (not file.is_open()) return;

        auto ar = sr::ReadArchive<std::ifstream>(file);

        Boy obj;

        std::string hi = "Hello!";
        char bye[10];

        ar & obj;
        ar & hi;
        ar & bye;

        file.close();

        std::cout << "Deserialization done.\n";

        std::cout << obj.name() << ' ' << obj.age() << ' ' << obj.force()
                  << ' ' << hi << ' ' << bye << '\n';
    }
}

void print(int** arr, int h, int w)
{
    for (int i = 0; i < h; ++i, std::cout << '\n')
        for (int j = 0; j < w; ++j)
            std::cout << arr[i][j] << ' ';

    std::cout << '\n';
}

void test_scope()
{
    {
        std::ofstream file("D:\\test_scope.bin");

        if (not file.is_open()) return;

        auto ar = sr::WriteArchive<std::ofstream>(file);

        int height = 3;
        unsigned int width = 4;

        int** tensor = new int* [height];
        for (int i = 0; i < width; ++i)
            tensor[i] = new int [width] { i, i + 1, i + 2, i + 3 };

        print(tensor, height, width);

        try
        {
            span(ar, tensor, height, width);
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }
        // implicit delete pointer
    }
    {
        std::ifstream file("D:\\test_scope.bin");

        if (not file.is_open()) return;

        auto ar = sr::ReadArchive<std::ifstream>(file);

        int height;
        int width;

        int** tensor = nullptr;

        try
        {
            span(ar, tensor, height, width);
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
            return;
        }

        print(tensor, height, width);

        // implicit delete pointer
    }
}

int main()
{
    test_common();
    test_std_array_serialization();

    test_object_serialization();

    test_scope();

    return 0;
}
