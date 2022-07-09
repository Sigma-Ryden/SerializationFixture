#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#include <Sifar/Core.hpp> // ReadAchive, WriteArchive

using sifar::ReadArchive;
using sifar::WriteArchive;

using sifar::base;

namespace some_namespace
{

class Parent
{
    SERIALIZATION_ACCESS()
    SERIALIZATION_POLYMORPHIC_KEY(0)

private:
    int a;

public:
    Parent() = default;
    Parent(int a) : a(a) {}

    virtual ~Parent() = default;

private:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & a;
    }
};

} // namespace some_namespace

// Class export MUST be in the global namespace
SERIALIZATION_EXPORT_KEY(some_namespace::Parent)

template <class T>
class Child : public some_namespace::Parent
{
    SERIALIZATION_ACCESS()

    // This is a default value for a class template, and using only for the 'sifar::InnerRegistry'
    SERIALIZATION_POLYMORPHIC_KEY(1)

private:
    int b;

public:
    Child() = default;
    Child(int a, int b) : Parent(a), b(b) {}

private:
    SERIALIZATION_UNIFIED(ar)
    {
        base<some_namespace::Parent>(ar, *this);
        ar & b;
    }
};

SERIALIZATION_EXPORT_TPL_KEY(2, Child<int>)

void test_class_export()
{
    using Parent = some_namespace::Parent;
    using Child  = Child<int>;

    {
        std::ofstream file("test_class_export.bin", std::ios::binary);
        if (not file.is_open()) return;

        WriteArchive<std::ofstream> ar(file);

        Parent* p = new Child(123, 321);

        try
        {
            ar & p;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
        }

        file.close();
        delete p;
    }
    {
        std::ifstream file("test_class_export.bin", std::ios::binary);
        if (not file.is_open()) return;

        ReadArchive<std::ifstream> ar(file);

        Parent* p = nullptr;

        try
        {
            ar & p;
        }
        catch (const char* e)
        {
            std::cout << e << '\n';
        }

        file.close();
        delete p;
    }
}

int main()
{
    test_class_export();

    return 0;
}
