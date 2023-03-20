#include "SirafTestingBase.hpp"

#include <Siraf/Support/shared_ptr.hpp>

using siraf::tracking::track;

TEST(TestLibrary, TestValidation)
{
    static int s_i = 745; // some garbage value

    std::vector<unsigned char> storage;

    {
        auto ar = oarchive(storage);

        int j = s_i;
        int* p_j = &j;

        ar & p_j;

        auto success = false;
        try { ar & track(j); } catch(...) { success = true; }

        EXPECT("bad data track", success);
    }

    storage.clear();
    {
        auto ar = oarchive(storage);

        int* g = nullptr; // ok - nullptr allowed
        ar & g;
    }
    {
        auto ar = iarchive(storage);

        int v = s_i;
        int* g = &v; // bad - should be nullptr

        auto success = false;
        try { ar & g; } catch(...) { success = true; }

        EXPECT("garbage ptr.read", success);
    }

    storage.clear();
    {
        auto ar = oarchive(storage);

        int* m = new int(s_i);

        int* r = m; // good
        std::shared_ptr<int> s(m); // bad - mixed tracking

        ar & r;

        auto success = false;
        try { ar & s; } catch(...) { success = true; }

        EXPECT("mixed track.raw shared", success);
    }

    storage.clear();
    {
        auto ar = oarchive(storage);

        int* m = new int(s_i);

        std::shared_ptr<int> s(m); // god
        int* r = m; // bad - mixed tracking

        ar & s;

        auto success = false;
        try { ar & r; } catch(...) { success = true; }

        EXPECT("mixed track.shared raw", success);
    }
}

class Triangle : POLYMORPHIC()
{
    SERIALIZABLE(Triangle)
};

class Cyrcle : POLYMORPHIC()
{
    SERIALIZABLE(Triangle) // oops - polymorphic key is already use
};

// is the same as below
//EXPORT_POLYMORPHIC_KEY("Triangle", Triangle) // good
//EXPORT_POLYMORPHIC_KEY("Triangle", Cyrcle) // bad - same keys

class Square // non cloneable
{
    SERIALIZABLE(Square)
};

TEST(TestLibrary, TestFactoryTable)
{
    using siraf::dynamic::FactoryRegistry;

    {
        FactoryRegistry<Triangle> triangle;

        auto success = false;
        try { FactoryRegistry<Cyrcle> cyrcle; } catch(...) { success = true; }

        EXPECT("same polymorphic key", success);
    }

    using siraf::dynamic::FactoryTable;
    using siraf::dynamic::ExternRegistry;

    auto& factory_table = FactoryTable::instance();
    auto key = ExternRegistry::key<Square>();

    {
        auto success = false;
        try { factory_table.update<Square>(key); } catch(...) { success = true; }

        EXPECT("non-cloneable type", success);
    }

    using siraf::memory::Raw;
    using siraf::memory::Shared;

    {
        auto success = false;
        try { factory_table.clone<Raw>(key); } catch(...) { success = true; }

        EXPECT("clone non-cloneable raw", success);
    }
    {
        auto success = false;
        try { factory_table.clone<Shared>(key); } catch(...) { success = true; }

        EXPECT("clone non-cloneable shared", success);
    }

    {
        auto r = new Square;

        auto success = false;
        try { (void)factory_table.cast(r, key); } catch(...) { success = true; }

        EXPECT("cast non-cloneable raw", success);

        delete r; // clean up
    }
    {
        auto s = std::make_shared<Square>();

        auto success = false;
        try { (void)factory_table.cast(s, key); } catch(...) { success = true; }

        EXPECT("cast non-cloneable shared", success);
    }
}

struct MyStruct : POLYMORPHIC()
{
    SERIALIZABLE(MyStruct)
};

class MyClass : POLYMORPHIC()
{
    SERIALIZABLE(MyClass)
};

struct MyCustomType : POLYMORPHIC()
{
    SERIALIZABLE(MyCustomType)
};

EXPORT_POLYMORPHIC_KEY("MyClass", MyStruct)
EXPORT_POLYMORPHIC_KEY("MyStruct", MyClass)

EXPORT_POLYMORPHIC(MyCustomType)

TEST(TestLibrary, TestExportPolymorphic)
{
    using siraf::core::Access;

    static auto sv_s = SIRAF_STATIC_HASH("MyClass");
    static auto sv_c = SIRAF_STATIC_HASH("MyStruct");

    {
        EXPECT("export polymorphic key.trait",
            Access::trait<MyStruct>() == sv_s &&
            Access::trait<MyClass>() == sv_c);

        EXPECT("export polymorphic key.static_trait",
            Access::static_trait<MyStruct>() == sv_c &&
            Access::static_trait<MyClass>() == sv_s);
    }

    using siraf::dynamic::ExternRegistry;

    static auto sv_ct = SIRAF_STATIC_HASH("MyCustomType");

    {
        // ExternRegistry::key<>() - is the same as Access::trait<>()
        EXPECT("export polymorphic",
            ExternRegistry::key<MyCustomType>() == sv_ct &&
            Access::trait<MyCustomType>() == sv_ct &&
            Access::static_trait<MyCustomType>() == sv_ct);
    }
}
