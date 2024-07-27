#include "SFTestingBase.hpp"

#include <SF/BuiltIn/string.hpp>

TEST_SPACE()
{

template <class SomeType>
struct Base : instantiable_t
{
    SERIALIZABLE(Base)

public:
    SomeType data;

public:
    Base() = default;
    Base(const SomeType& data) : data(data) {}
};

template <typename> struct is_base : std::false_type {};
template <typename T> struct is_base<Base<T>> : std::true_type {};

namespace internal // example namespace
{

struct Derived : Base<std::string>
{
    SERIALIZABLE(internal::Derived)

public:
    float value;

public:
    Derived() = default;
    Derived(const std::string& data, float c) : Base(data), value(c) {}
};

} // namespace internal

} // TEST_SPACE

SERIALIZATION(Save, Base<std::string>)
{
    // more specialize version
    archive & self.data;
}

CONDITIONAL_SERIALIZATION(SaveLoad, is_base<T>::value)
{
    archive & self.data;
}

SERIALIZATION(SaveLoad, internal::Derived)
{
    archive & hierarchy<Base<std::string>>(self);
    archive & self.value;
}

// we can overload polymorphic key for each class (or full template specialization)

// smae as EXPORT_INSTANTIABLE_KEY("Base<double>", Base<double>)
EXPORT_INSTANTIABLE(Base<double>)
EXPORT_INSTANTIABLE(Base<std::string>)

// EXPORT_INSTANTIABLE(internal::Derived) // - not required, but allow
// EXPORT_INSTANTIABLE_KEY("Other Derived", "internal::Derived) // - possible use

TEST(TestCommon, TestPolymorphic)
{
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
        ASSERT("polymorphic.traits", xxsf::traits<Parent>() == xxsf::traits(*p));

        EXPECT("polymorphic.value", p->data == sv_p_d);

        auto d = dynamic_cast<Child*>(c);

        ASSERT("polymorphic.derived inited", d != nullptr);
        ASSERT("polymorphic.derived traits", xxsf::traits<Child>() == xxsf::traits(*c));

        EXPECT("polymorphic.derived value", d->data == sv_c_d && d->value == sv_c_v);
    }
}
