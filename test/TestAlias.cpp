#include "SFTestingBase.hpp"

#include <SF/Utility/Alias.hpp>

TEST_SPACE()
{

struct Base : instantiable_t
{
    SERIALIZABLE(Base)
    int x;
};

struct Derived : Base
{
    SERIALIZABLE(Derived)
    int y;
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, Base)
{
    archive & self.x;
}

SERIALIZATION(SaveLoad, Derived)
{
    archive & hierarchy<Base>(self) & self.y;
}

TEST(TestCommon, TestAlias)
{
    using sf::alias_t;
    using sf::tracking::track;

    static int s_i = 64822;

    std::vector<unsigned char> storage;
    {
        int i = s_i;
        alias_t<int> r1 = i;
        alias_t<int> r2 = i;
        alias_t<int> r3 = r2;

        auto ar = oarchive(storage);
        ar & track(i) & r1 & r2 & r3;
    }
    {
        int i;
        alias_t<int> r1;
        alias_t<int> r2;
        alias_t<int> r3;

        auto ar = iarchive(storage);
        ar & track(i) & r2 & r3 & r1; // one more shuffle

        ASSERT("inited", r1.is_refer(i) && r2.is_refer(i) && r3.is_refer(i));
        EXPECT("value", i == s_i);
    }

    static Derived s_d;
    s_d.x = 75312;
    s_d.y = 3921;

    storage.clear();
    {
        Derived d = s_d;
        alias_t<Derived> rd = d;
        alias_t<Base> rb1 = rd;
        alias_t<Base> rb2 = alias_t<Derived>(d);

        auto ar = oarchive(storage);
        ar & track(d) & rd & rb1 & rb2;
    }
    {
        Derived d;
        alias_t<Derived> rd;
        alias_t<Base> rb1;
        alias_t<Base> rb2;

        auto ar = iarchive(storage);
        ar & track(d) & rb2 & rd & rb1; // one more shuffle

        ASSERT("polymorphic inited", rd.is_refer(d) && rb1.is_refer(d) && rb2.is_refer(d));
        EXPECT("polymorphic value", d.x == s_d.x && d.y == s_d.y);
    }
}
