#if __cplusplus >= 201703L

#include <SFTestingBase.hpp>

#include <SF/BuiltIn/variant.hpp>
#include <SF/BuiltIn/string.hpp>

TEST_SPACE()
{

struct OwnType
{
    OwnType(int data) : var(data) {}

    int var;
};

} // TEST_SPACE

SERIALIZATION(saveload, self, OwnType)
{
    archive & self.var;
}

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
