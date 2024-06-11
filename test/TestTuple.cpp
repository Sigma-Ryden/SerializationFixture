#include "SFTestingBase.hpp"

#include <SF/BuiltIn/tuple.hpp>
#include <SF/BuiltIn/string.hpp>

TEST(TestUtility, TestTuple)
{
    enum class Charge { Low, Medium, High };

    static std::tuple<> s_et;
    static std::tuple<std::string, double, Charge> s_t{ "Tuple", 123.456, Charge::Low };

    std::vector<unsigned char> storage;
    {
        std::tuple<> et = s_et;
        std::tuple<std::string, double, Charge> t = s_t;

        auto ar = oarchive(storage);
        ar & et & t;
    }
    {
        std::tuple<> et;
        std::tuple<std::string, double, Charge> t;

        auto ar = iarchive(storage);
        ar & et & t;

        EXPECT("std::tuple<empty>", et == s_et);
        EXPECT("std::tuple<>", t == s_t);
    }
}
