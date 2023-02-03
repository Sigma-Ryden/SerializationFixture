#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Siraf/Core.hpp>
#include <Siraf/Support/string.hpp>

using namespace siraf::library; // support of string

class Shape
{
    SERIALIZABLE(Shape)

private:
    std::string name_;
    float x_;
    float y_;

public:
    Shape() : name_("Unknown"), x_(), y_() {}

    Shape(const char* name, float x, float y)
        : name_(name), x_(x), y_(y) {}

    const std::string& name() const { return name_; }
    float x() const { return x_; }
    float y() const { return y_; }
};

SERIALIZATION(SaveLoad, Shape)
{
    archive & self.name_
            & self.x_
            & self.y_;
}

void save(Shape& shape)
{
    std::ofstream file("example.bin", std::ios::binary);
    if (not file.is_open()) return;

    auto ar = siraf::oarchive(file);

    ar & shape;

    file.close();
}

void load(Shape& shape)
{
    std::ifstream file("example.bin", std::ios::binary);
    if (not file.is_open()) return;

    auto ar = siraf::iarchive(file);

    ar & shape;

    file.close();
}

void info(const Shape& shape)
{
    std::cout << shape.name() << " shape is lacated at: "
              << shape.x() << "; " << shape.y() << std::endl;
};

int main()
{
    // Saving of Shape object
    {
        Shape shape("Square", 0.25f, 2.5f);
        save(shape);
    }
    // Loading of Shape object
    {
        Shape shape;
        info(shape); // before load

        load(shape);

        info(shape); // after load
    }

    return 0;
}
