#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp>
#include <Sifar/Support/string.hpp>

using namespace sifar::library; // support of string

class Shape
{
    SERIALIZABLE()

private:
    std::string name_;
    float x_, y_;

public:
    Shape() : name_("Unknown"), x_(), y_() {}

    Shape(const char* name, float x, float y)
        : name_(name), x_(x), y_(y) {}

    void print() const
    {
        std::cout << name_ << " shape is lacated at: "
                  << x_ << "; " << y_ << std::endl;
    }
};

SAVE_LOAD_SERIALIZABLE(Shape)
{
    archive & self.name_
            & self.x_
            & self.y_;
}

void save(Shape& shape)
{
    std::ofstream file("example.bin", std::ios::binary);
    if (not file.is_open()) return;

    auto ar = sifar::writer(file);

    ar & shape;

    file.close();
}

void load(Shape& shape)
{
    std::ifstream file("example.bin", std::ios::binary);
    if (not file.is_open()) return;

    auto ar = sifar::reader(file);

    ar & shape;

    file.close();
}

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
        shape.print();

        load(shape);

        shape.print();
    }

    return 0;
}
