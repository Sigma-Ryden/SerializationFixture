#include "TestTuple.h"

#include <Siraf/Support/tuple.hpp>
#include <Siraf/Support/string.hpp>

DEFINE_AUTO_TEST(TestTuple)
{
    enum class Charge { Low, Medium, High };

    static std::tuple<> s_et;
    static std::tuple<std::string, double, Charge> s_t{ "Tuple", 123.456, Charge::Low };

    std::vector<unsigned char> storage;
    {
        std::tuple<> et = s_et;
        std::tuple<std::string, double, Charge> t = s_t;

        auto ar = oarchive<OByteStream>(storage);
        ar & et & t;
    }
    {
        std::tuple<> et;
        std::tuple<std::string, double, Charge> t;

        auto ar = iarchive<IByteStream>(storage);
        ar & et & t;

        EXPECT("std::tuple<empty>", et == s_et);
        EXPECT("std::tuple<>", t == s_t);
    }
}
