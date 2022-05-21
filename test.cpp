#include <iostream> // cin, cout
#include <fstream> // ifstream, ofstream

#include <array> // array
#include <string> // string

#include "Serialization/Core.hpp"

namespace sr = serialization;

using namespace sr::support::common;
using namespace sr::support::library;

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

template <typename InStream>
class FormatInStreamWrapper
{
private:
    InStream& stream_;

public:
    FormatInStreamWrapper(InStream& stream) : stream_(stream) {}

    template <typename T>
    FormatInStreamWrapper& read(T& data, std::size_t /*n*/)
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
    FormatOutStreamWrapper& write(const T& data, std::size_t /*n*/)
    {
        stream_ << data << ' ';

        return *this;
    }
};

template <typename OutStream>
using FormatWriteArchive = sr::WriteArchive<OutStream, FormatOutStreamWrapper<OutStream>>;

// FormatWriteArchive overloading operator& for class B
template <typename OutStream>
auto operator& (FormatWriteArchive<OutStream>& archive, B& t) -> decltype(archive)
{
    std::cout << "overloading operator& for B";

    return archive;
}

template <typename> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

// user definition of std::array serialization
SERIALIZATION_WRITE_ARCHIVE_GENERIC(array, is_std_array<T>::value)
{
    using value_type = typename T::value_type;

    auto array_size = array.size();

    archive & array_size;
    for (auto item : array) archive & item;

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

    auto scope_boo = sr::scope(boo);
    auto scope_tensor = sr::scope(dynamic_tensor, sr::dim(height, width));
    auto scope_array = sr::scope(dynamic_array, width);

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

        auto ar = sr::write_archive(file);

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

        auto ar = sr::read_archive(file);

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

int main()
{
    test_object_serialization();

    return 0;
}
