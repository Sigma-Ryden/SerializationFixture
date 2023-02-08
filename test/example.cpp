#include <Siraf/Core.hpp>

#include <Siraf/Support/string.hpp>
#include <Siraf/Support/vector.hpp>
#include <Siraf/Support/map.hpp>
#include <Siraf/Support/shared_ptr.hpp>

enum class Property
{
    Speed,
    Force,
    Intelige,
    Fly
};

struct Prototype
{
    SERIALIZABLE(Prototype)

    std::string name;
    std::vector<Property> properties;
};

struct Handbook
{
    SERIALIZABLE(Handbook)

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
        using siraf::oarchive;
        using siraf::wrapper::OByteStream;

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

        auto ar = oarchive<OByteStream>(storage);
        ar & db;
    }
    // load
    {
        using siraf::iarchive;
        using siraf::wrapper::IByteStream;

        Handbook db;

        auto ar = iarchive<IByteStream>(storage);
        ar & db;
    }

    return 0;
}
