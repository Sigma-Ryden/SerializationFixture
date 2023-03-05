#if __cplusplus >= 201703L

#include "SirafTestingBase.hpp"

#include <Siraf/Support/variant.hpp>
#include <Siraf/Support/string.hpp>

struct OwnType
{
    OwnType(int data) : var(data) {}

    int var;
};

SERIALIZATION(SaveLoad, OwnType)
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

        ASSERT("std::variant<>", v.index() == 1);
        EXPECT("std::variant<>", std::get<std::string>(v) == sv_v);

        ASSERT("std::variant<monostate>", m.index() == 0);
    }
}

#endif // if
