#if __cplusplus >= 201703L

#include "SirafTestingBase.hpp"

#include <Siraf/Support/variant.hpp>

TEST(TestUtility, TestVariant)
{
    std::vector<unsigned char> storage;
    {
        std::variant<float> v;

        auto ar = oarchive(storage);
        ar & v;
    }
    {
        std::variant<float> v;

        auto ar = iarchive(storage);
        ar & v;

        EXPECT("std::variant<>", false);
    }
}

#endif // if
