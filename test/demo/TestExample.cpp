#include "SFTestingBase.hpp"

#include <SF/Support/string.hpp>
#include <SF/Support/vector.hpp>
#include <SF/Support/map.hpp>
#include <SF/Support/shared_ptr.hpp>

TEST_SPACE()
{

enum class Property
{
    Speed,
    Force,
    inteligance,
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

} // TEST_SPACE

SERIALIZATION(SaveLoad, Prototype)
{
    archive & self.name & self.properties;
}

SERIALIZATION(SaveLoad, Handbook)
{
    archive & self.prototypes;
}

TEST(TestDemo, TestExample)
{
    static std::string s_zero_name = "Zero";
    static std::string s_rew_name = "Rew";
    static std::string s_ifly_name = "I.Fly";

    static std::vector<Property> s_zero_properties = {Property::Speed};
    static std::vector<Property> s_rew_properties = {Property::Force};
    static std::vector<Property> s_ifly_properties = {Property::inteligance, Property::Fly};

    std::vector<unsigned char> storage;
    {
        Handbook db;

        auto zero = std::make_shared<Prototype>();
        zero->name = s_zero_name;
        zero->properties = s_zero_properties;

        auto rew = std::make_shared<Prototype>();
        rew->name = s_rew_name;
        rew->properties = s_rew_properties;

        auto ifly = std::make_shared<Prototype>();
        ifly->name = s_ifly_name;
        ifly->properties = s_ifly_properties;

        db.prototypes[0] = zero;
        db.prototypes[3] = rew;
        db.prototypes[2] = ifly;

        auto ar = sf::oarchive(storage);
        ar & db;
    }
    {
        Handbook db;

        auto ar = sf::iarchive(storage);
        ar & db;

        auto zero = db.prototypes[0];
        auto rew = db.prototypes[3];
        auto ifly = db.prototypes[2];

        ASSERT("inited", zero != nullptr && rew != nullptr && ifly != nullptr);
        EXPECT("value",
            zero->name == s_zero_name && zero->properties == s_zero_properties &&
            rew->name == s_rew_name && rew->properties == s_rew_properties &&
            ifly->name == s_ifly_name && ifly->properties == s_ifly_properties);
    }
}
