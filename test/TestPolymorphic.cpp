#include <SFTestingBase.hpp>

#include <SF/BuiltIn/string.hpp>

TEST_SPACE()
{

template <class SomeType>
struct Base : instantiable_t
{
public:
    SomeType data;

public:
    Base() = default;
    Base(const SomeType& data) : data(data) {}
};

namespace internal // example namespace
{

struct Derived : Base<std::string>
{
public:
    float value;

public:
    Derived() = default;
    Derived(const std::string& data, float c) : Base(data), value(c) {}
};

} // namespace internal

} // TEST_SPACE

TEMPLATE_SERIALIZABLE_DECLARATION(template <class SomeType>, Base<SomeType>)
    INSTANTIABLE(S)
SERIALIZABLE_DECLARATION_INIT()

TEMPLATE_SERIALIZABLE(saveload, self, template <class SomeType>, Base<SomeType>)
    // more specialize version
    archive & self.data;
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(internal::Derived)
    INSTANTIABLE(S)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, internal::Derived)
    archive & hierarchy<Base<std::string>>(self);
    archive & self.value;
SERIALIZABLE_INIT()

// INSTANTIABLE(Base<double>) // same as INSTANTIABLE_KEY(SF_TYPE_HASH(Base<double>), Base<double>)
// INSTANTIABLE_KEY(SF_STRING_HASH("Other Derived"), internal::Derived) // possible use

TEST(TestCommon, TestPolymorphic)
{
    sf::serializable<Base<double>>();
    sf::serializable<Base<std::string>>();

    using Parent = Base<std::string>;
    using Child  = internal::Derived;

    static std::string sv_p_d = "Fuzz";

    static std::string sv_c_d = "Buzz";
    static float sv_c_v = 1291.238f;

    std::vector<unsigned char> storage;
    {
        Parent* p = new Parent(sv_p_d);
        Parent* c = new Child(sv_c_d, sv_c_v);

        auto ar = oarchive(storage);
        ar & p & c;
    }
    {
        Parent* p = nullptr;
        Parent* c = nullptr;

        auto ar = iarchive(storage);
        ar & p & c;

        ASSERT("polymorphic.inited", p != nullptr);
        ASSERT("polymorphic.traits", SF_TYPE_HASH(Parent) == SF_EXPRESSION_HASH(*p));

        EXPECT("polymorphic.value", p->data == sv_p_d);

        auto d = dynamic_cast<Child*>(c);

        ASSERT("polymorphic.derived inited", d != nullptr);
        ASSERT("polymorphic.derived traits", SF_TYPE_HASH(Child) == SF_EXPRESSION_HASH(*c));

        EXPECT("polymorphic.derived value", d->data == sv_c_d && d->value == sv_c_v);
    }
}
