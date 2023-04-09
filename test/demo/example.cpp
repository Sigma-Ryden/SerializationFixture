#include <SF/Core.hpp>

#include <SF/Support/string.hpp>
#include <SF/Support/vector.hpp>
#include <SF/Support/map.hpp>
#include <SF/Support/shared_ptr.hpp>

enum class Property
{
    Speed,
    Force,
    Intelige,
    Fly
};

struct Prototype
{
    std::string name;
    std::vector<Property> properties;
};

struct Handbook
{
    std::map<int, std::shared_ptr<Prototype>> prototypes;
};

SERIALIZATION(SaveLoad, Prototype)
{
    archive & self.name & self.properties;
}

SERIALIZATION(SaveLoad, Handbook)
{
    archive & self.prototypes;
}

int main()
{
    std::vector<unsigned char> storage;

    // save
    {
        Handbook db;

        auto zero = std::make_shared<Prototype>();
        zero->name = "Zero";
        zero->properties = {Property::Speed};

        auto rew = std::make_shared<Prototype>();
        rew->name = "Rew";
        rew->properties = {Property::Force};

        auto ifly = std::make_shared<Prototype>();
        ifly->name = "I.Fly";
        ifly->properties = {Property::Intelige, Property::Fly};

        db.prototypes[0] = zero;
        db.prototypes[3] = rew;
        db.prototypes[2] = ifly;

        auto ar = sf::oarchive(storage);
        ar & db;
    }
    // load
    {
        Handbook db;

        auto ar = sf::iarchive(storage);
        ar & db;
    }

    return 0;
}
