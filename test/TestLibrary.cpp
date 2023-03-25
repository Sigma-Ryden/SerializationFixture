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

#include <Siraf/Support/string.hpp>

TEST(TestLibrary, TestStreamWrapper)
{
    static std::string s_s = "Hello, World!";

    std::vector<char> storage;
    {
        std::string s = s_s;

        auto ar = oarchive(storage);
        ar & s;
    }
    {
        std::string s;

        auto ar = iarchive(storage);
        ar & s;

        EXPECT("char storage", s == s_s);
    }

    storage.clear();
    {
        std::string s = s_s;

        auto ar = oarchive(storage);
        ar & s;

        std::ofstream file("test.bin", std::ios::binary);

        // if we use unsigned char - we should use reinterpret_cast to const char
        // or 'siraf::memory::const_byte_cast' function
        file.write(storage.data(), storage.size());

        file.close();
    }
    {
        std::string s;

        std::ifstream file("test.bin", std::ios::binary);

        auto ar = iarchive<siraf::wrapper::IFileStream>(file);
        ar & s;

        EXPECT("storage converting", s == s_s);
    }
}

// if you won't serialize object just omit SERIlALIZABLE(type)
// but if you are going to serialize it in the future -
// write SERIALIZABLE(type) and omit SERIALIZATION(mode, type)

struct WorldObject : siraf::dynamic::Polymorphic
{
    SERIALIZABLE(WorldObject)
    int wo = 0;
};

SERIALIZATION(SaveLoad, WorldObject)
{
    ++self.wo;
}

struct EnvironmentObject : virtual WorldObject
{
    SERIALIZABLE(EnvironmentObject)
    int eo = 0;
};

SERIALIZATION(SaveLoad, EnvironmentObject)
{
    ++self.eo;
    archive & hierarchy<WorldObject>(self);
}

struct MoveableObject : virtual EnvironmentObject
{
    SERIALIZABLE(MoveableObject)
    int mo = 0;
};

SERIALIZATION(SaveLoad, MoveableObject)
{
    ++self.mo;
    archive & hierarchy<EnvironmentObject>(self);
}

struct DestructibleObject : virtual EnvironmentObject
{
    SERIALIZABLE(DestructibleObject)
    int dso = 0;
};

SERIALIZATION(SaveLoad, DestructibleObject)
{
    ++self.dso;
    archive & hierarchy<EnvironmentObject>(self);
}

struct DecorativeObject : DestructibleObject, MoveableObject
{
    SERIALIZABLE(DecorativeObject)
    int dco = 0;
};

SERIALIZATION(SaveLoad, DecorativeObject)
{
    ++self.dco;
    archive & hierarchy<DestructibleObject, MoveableObject>(self);
}

struct FoliageObject : virtual WorldObject
{
    SERIALIZABLE(FoliageObject)
    int fo = 0;
};

struct FoliageObjectInstance : FoliageObject {};

SERIALIZATION(SaveLoad, FoliageObject)
{
    ++self.fo;
    archive & hierarchy<WorldObject>(self);
}

struct DecorativeFoliageObject : DecorativeObject, FoliageObjectInstance
{
    SERIALIZABLE(DecorativeFoliageObject)
    int dcfo = 0;
};

SERIALIZATION(SaveLoad, DecorativeFoliageObject)
{
    ++self.dcfo;
    archive & hierarchy<DecorativeObject, FoliageObject>(self);
}

TEST(TestLibrary, TestInheritance)
{
    auto check_hierarchy_count = [](std::shared_ptr<DecorativeFoliageObject>& d)
    {
        return d->wo == 1 && d->eo == 1 && d->mo == 1 && d->dso == 1 && d->dco == 1 && d->dcfo == 1;
    };

    std::vector<unsigned char> storage;
    {
        auto d = std::make_shared<DecorativeFoliageObject>();
        std::shared_ptr<WorldObject> b = d;

        auto ar = oarchive(storage);
        ar & b;

        EXPECT("write.hierarchy count", check_hierarchy_count(d));
    }
    {
        std::shared_ptr<WorldObject> b = nullptr;

        auto ar = iarchive(storage);
        ar & b;

        auto d = std::dynamic_pointer_cast<DecorativeFoliageObject>(b);

        ASSERT("read.inited", d != nullptr);
        EXPECT("read.hierarchy count", check_hierarchy_count(d));
    }
}
