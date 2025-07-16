#include <EightserTestingBase.hpp>

#include <Eightser/BuiltIn/valarray.hpp>

template <typename T>
bool is_equal(const std::valarray<T>& lhs, const std::valarray<T>& rhs)
{
    return std::equal(std::begin(lhs), std::end(lhs), std::begin(rhs));
}

TEST(TestNumeric, TestValarray)
{
    static std::valarray<float> s_v = { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f };

    std::vector<unsigned char> storage;
    {
        std::valarray<float> v = s_v;

        auto ar = oarchive(storage);
        ar & s_v;
    }
    {
        std::valarray<float> v;

        auto ar = iarchive(storage);
        ar & v;

        EXPECT("std::valarray<>", is_equal(v, s_v));
    }
}
