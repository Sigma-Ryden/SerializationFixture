#if __cplusplus >= 201703L

#include "SFTestingBase.hpp"

#include <SF/Support/string.hpp>
#include <SF/Support/any.hpp>
#include <SF/Support/shared_ptr.hpp>

struct AggregateType
{
    std::string string;
    std::any data;

    struct InnerType
    {
        char symbol;
        float constant;
    };

    std::shared_ptr<InnerType> inner;
};

TEST(TestCommon, TestAggregate)
{
    using sf::serializable; // do possible serialize data assigned to any

    static std::string s_string = "Hello, Aggregate!";
    static double s_data = 123.321;

    static char s_symbol = 's';
    static float s_constant = 2.17f;

    std::vector<unsigned char> storage;
    {
        AggregateType::InnerType inner { s_symbol, s_constant };
        AggregateType at { s_string, serializable(s_data), std::make_shared<AggregateType::InnerType>(inner) };

        auto ar = oarchive(storage);
        ar & at;
    }
    {
        AggregateType at;

        auto ar = iarchive(storage);
        ar & at;

        auto data = std::any_cast<double>(&at.data);

        EXPECT("inited", data != nullptr && at.inner != nullptr);
        EXPECT("value", at.string == s_string && *data == s_data &&
            at.inner->symbol == s_symbol && at.inner->constant == s_constant);
    }
}

#endif // if
