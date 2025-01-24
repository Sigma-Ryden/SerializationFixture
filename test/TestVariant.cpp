#if __cplusplus >= 201703L

#include <SFTestingBase.hpp>

#include <SerializationFixture/BuiltIn/variant.hpp>
#include <SerializationFixture/BuiltIn/string.hpp>

TEST_SPACE()
{

struct OwnType
{
    OwnType(int data) : var(data) {}

    int var;
};

} // TEST_SPACE

SERIALIZABLE_DECLARATION(OwnType)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, OwnType)
    SERIALIZATION
    (
        archive & self.var;
    )
SERIALIZABLE_INIT()

TEST(TestUtility, TestVariant)
{
    std::string sv_v = "Variant";

    std::vector<unsigned char> storage;
    {
        std::variant<float, std::string> v = sv_v;
        std::variant<std::monostate, OwnType> m;

        auto ar = oarchive(storage);
        ar & v & m;
    }
    {
        std::variant<float, std::string> v;
        std::variant<std::monostate, OwnType> m;

        auto ar = iarchive(storage);
        ar & v & m;

        ASSERT("std::variant<>.index", v.index() == 1);
        EXPECT("std::variant<>.get", std::get<std::string>(v) == sv_v);

        ASSERT("std::variant<monostate>.index", m.index() == 0);
    }
}

#endif // if
