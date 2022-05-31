#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Serialization/Core.hpp>
#include <Serialization/Support/string.hpp>

using namespace serialization::library; // support of string

using serialization::WriteArchive;
using serialization::ReadArchive;

class Shape
{
    SERIALIZATION_ARCHIVE_ACCESS()

private:
    std::string name_;
    float x_, y_;

public:
    Shape() : name_("UNNAMED"), x_(0), y_(0) {}

    Shape(const std::string& name, float x,  float y)
        : name_(name), x_(x), y_(y) {}

    void print() const
    {
        std::cout << name_ << " shape is lacated at: ("
                  << x_ << ", " << y_ << ")\n";
    }

private:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & name_;
        ar & x_;
        ar & y_;

        return ar;
    }
};

void save(Shape& shape)
{
    std::ofstream file("example.bin", std::ios::binary);

    if (not file.is_open()) return;

    WriteArchive<std::ofstream> ar(file);

    ar & shape;

    file.close();
}

void load(Shape& shape)
{
    std::ifstream file("example.bin", std::ios::binary);

    if (not file.is_open()) return;

    ReadArchive<std::ifstream> ar(file);

    ar & shape;

    file.close();
}

int main()
{
    // Saving of Shape object
    {
        Shape shape("Square", 0.1234f, 5.6789f);
        save(shape);
    }
    // Loading of Shape object
    {
        Shape shape;
        shape.print();

        load(shape);

        shape.print();
    }

    return 0;
}
