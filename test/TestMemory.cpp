#include "SirafTestingBase.hpp"

#include <Siraf/Support/unique_ptr.hpp>

struct Parent : POLYMORPHIC()
{
    SERIALIZABLE(Parent)

    virtual ~Parent() = default;
    int p = 0;
};

SERIALIZATION(SaveLoad, Parent)
{
    archive & self.p;
}

struct Child : Parent
{
    SERIALIZABLE(Child)

    int c = 0;
};

SERIALIZATION(SaveLoad, Child)
{
    archive & hierarchy<Parent>(self) & self.c;
}

bool operator== (const Child& lhs, const Child& rhs)
{
    return lhs.p == rhs.p && lhs.c == rhs.c;
}

TEST(TestMemory, TestUniquePtr)
{
    static int sv_i = 8791;

    Child sv_c;
    sv_c.p = 506;
    sv_c.c = 1234;

    std::vector<unsigned char> storage;
    {
        std::unique_ptr<int> u_i(new int(sv_i));
        std::unique_ptr<Parent> u_p(new Child(sv_c));

        auto ar = oarchive<OByteStream>(storage);
        ar & u_i & u_p;
    }
    {
        std::unique_ptr<int> u_i = nullptr;
        std::unique_ptr<Parent> u_p = nullptr;

        auto ar = iarchive<IByteStream>(storage);
        ar & u_i & u_p;

        EXPECT("std::unique_ptr<>", u_i != nullptr && *u_i == sv_i);

        auto raw_u_p = u_p.get();
        auto raw_u_c = dynamic_cast<Child*>(raw_u_p);

        EXPECT("std::unique_ptr<polymorphic>", raw_u_p != nullptr && raw_u_c != nullptr && *raw_u_c == sv_c);
    }
}

#include <Siraf/Support/shared_ptr.hpp>

TEST(TestMemory, TestSharedPtr)
{
    EXPECT("std::shared_ptr<>", false);
}

#include <Siraf/Support/weak_ptr.hpp>

TEST(TestMemory, TestWeakPtr)
{
    static int sv_i = 983258;

    Child sv_c;
    sv_c.p = 7458;
    sv_c.c = 589;

    std::vector<unsigned char> storage;
    {
        auto s_i = std::make_shared<int>(sv_i);
        auto s_p = std::make_shared<Child>(sv_c);

        std::weak_ptr<int> w_i = s_i;
        std::weak_ptr<Parent> w_p = s_p;

        auto ar = oarchive<OByteStream>(storage);
        ar & w_i & w_p;
    }
    {
        std::unique_ptr<int> w_i = nullptr;
        std::unique_ptr<Parent> w_p = nullptr;

        auto ar = iarchive<IByteStream>(storage);
        ar & w_i & w_p;

        EXPECT("std::weak_ptr<>", w_i != nullptr && *w_i == sv_i);

        auto raw_w_p = w_p.get();
        auto raw_w_c = dynamic_cast<Child*>(raw_w_p);

        EXPECT("std::weak_ptr<polymorphic>", raw_w_p != nullptr && raw_w_c != nullptr && *raw_w_c == sv_c);
    }
}
