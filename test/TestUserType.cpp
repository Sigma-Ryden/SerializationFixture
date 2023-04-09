#include "SFTestingBase.hpp"

struct Vector
{
    //SERIALIZABLE(Vector) // not required

    Vector(float x = 0.f, float y = 0.f, float z = 0.f)
        : X(x), Y(y), Z(z) {}

    float X;
    float Y;
    float Z;
};

struct Box
{
    //SERIALIZABLE(Box) // not required

    Box() {}
    Box(const Vector& min, const Vector& max)
        : Min(min), Max(max) {}

    Vector Min;
    Vector Max;
};

SERIALIZATION(SaveLoad, Vector)
{
    archive & self.X & self.Y & self.Z;
}

SERIALIZATION(SaveLoad, Box)
{
    archive & self.Min & self.Max;
}

TEST(TestCommon, TestUserType)
{
    static Vector s_min(0.1f, 1.3f, 2.1f);
    static Vector s_max(3.2f, 2.f, 3.5f);

    static auto equal = [](const Vector& A, const Vector& B)
    {
        return A.X == B.X && A.Y == B.Y && A.Z == B.Z;
    };

    std::vector<unsigned char> storage;
    {
        Box box(s_min, s_max);

        auto ar = oarchive(storage);
        ar & box;
    }
    {
        Box box;

        auto ar = iarchive(storage);
        ar & box;

        EXPECT("Struct", equal(box.Min, s_min) && equal(box.Max, s_max));
    }
}

#include <SF/Support/string.hpp>
#include <SF/Support/any.hpp>

struct Product
{
    SERIALIZABLE(Product)

    std::string name;
    std::size_t series;
    std::size_t price;
};

SERIALIZATION(SaveLoad, Product)
{
    archive & self.name & self.series & self.price;
}

struct Printer : Product
{
    SERIALIZABLE(Printer)

    std::any owner;
};

SERIALIZATION(SaveLoad, Printer)
{
    archive & hierarchy<Product>(self) & self.owner;
}

TEST(TestCommon, TestInheritance)
{
    static std::string s_owner = "Jen";

    static Printer s_p;
    s_p.name = "Canon";
    s_p.series = 37868723;
    s_p.price = 1000;
    s_p.owner = sf::serializable(s_owner);

    std::vector<unsigned char> storage;
    {
        Printer p = s_p;

        auto ar = oarchive(storage);
        ar & p;
    }
    {
        Printer p = s_p;

        auto ar = iarchive(storage);
        ar & p;

        auto owner = std::any_cast<std::string>(&p.owner);
        EXPECT("inheritance.value",
            p.name == s_p.name && p.series == s_p.series && p.price == s_p.price &&
            owner != nullptr && *owner == s_owner);
    }
}
