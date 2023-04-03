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

class Triangle : Instantiable
{
    SERIALIZABLE(Triangle)
};

class Cyrcle : Instantiable
{
    SERIALIZABLE(Triangle) // oops - polymorphic key is already use
};

// is the same as below
//EXPORT_POLYMORPHIC_KEY("Triangle", Triangle) // good
//EXPORT_POLYMORPHIC_KEY("Triangle", Cyrcle) // bad - same keys

class Square // non instantiable
{
    SERIALIZABLE(Square)
};

TEST(TestLibrary, TestFactoryTable)
{
    using siraf::dynamic::InstantiableFixture;

    {
        InstantiableFixture<Triangle> triangle;

        auto success = false;
        try { InstantiableFixture<Cyrcle> cyrcle; } catch(...) { success = true; }

        EXPECT("same polymorphic key", success);
    }

    using siraf::dynamic::InstantiableRegistry;
    using siraf::dynamic::ExternRegistry;

    auto& registry = InstantiableRegistry::instance();
    auto key = ExternRegistry::key<Square>();

    {
        auto success = false;
        try { registry.update<Square>(key); } catch(...) { success = true; }

        EXPECT("non-instantiable type", success);
    }

    using siraf::Memory;

    {
        auto success = false;
        try { registry.clone<Memory::Raw>(key); } catch(...) { success = true; }

        EXPECT("clone non-instantiable raw", success);
    }
    {
        auto success = false;
        try { registry.clone<Memory::Shared>(key); } catch(...) { success = true; }

        EXPECT("clone non-instantiable shared", success);
    }

    {
        auto r = new Square;

        auto success = false;
        try { (void)registry.cast(r, key); } catch(...) { success = true; }

        EXPECT("cast non-instantiable raw", success);

        delete r; // clean up
    }
    {
        auto s = std::make_shared<Square>();

        auto success = false;
        try { (void)registry.cast(s, key); } catch(...) { success = true; }

        EXPECT("cast non-instantiable shared", success);
    }
}

struct MyStruct : Instantiable
{
    SERIALIZABLE(MyStruct)
};

class MyClass : siraf::Instantiable // is same as Instantiable
{
    SERIALIZABLE(MyClass)
};

struct MyCustomType : Instantiable
{
    SERIALIZABLE(MyCustomType)
};

EXPORT_POLYMORPHIC_KEY("MyClass", MyStruct)
EXPORT_POLYMORPHIC_KEY("MyStruct", MyClass)

EXPORT_POLYMORPHIC(MyCustomType)

TEST(TestLibrary, TestExportPolymorphic)
{
    static auto sv_s = SIRAF_STATIC_HASH("MyClass");
    static auto sv_c = SIRAF_STATIC_HASH("MyStruct");

    {
        EXPECT("export polymorphic key.trait",
            Serialization::trait<MyStruct>() == sv_s &&
            Serialization::trait<MyClass>() == sv_c);

        EXPECT("export polymorphic key.static_trait",
            Serialization::static_trait<MyStruct>() == sv_c &&
            Serialization::static_trait<MyClass>() == sv_s);
    }

    using siraf::dynamic::ExternRegistry;

    static auto sv_ct = SIRAF_STATIC_HASH("MyCustomType");

    {
        // ExternRegistry::key<>() - is the same as Serialization::trait<>()
        EXPECT("export polymorphic",
            ExternRegistry::key<MyCustomType>() == sv_ct &&
            Serialization::trait<MyCustomType>() == sv_ct &&
            Serialization::static_trait<MyCustomType>() == sv_ct);
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

struct WorldObject : Instantiable
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
//
class PolymorphicBaseImpl : public siraf::Instantiable
{
    SERIALIZABLE(PolymorphicBaseImpl)

public:
    PolymorphicBaseImpl() = default;
    PolymorphicBaseImpl(int data) : data_(data) {}

protected:
    int data_;
};

SERIALIZATION(SaveLoad, PolymorphicBaseImpl)
{
    archive & self.data_;
}

class PolymorphicBase : public PolymorphicBaseImpl // try protected inheritance
{
    SERIALIZABLE(PolymorphicBase)

public:
    PolymorphicBase(int data = 0) : PolymorphicBaseImpl(data), inner_data_(data/2) {}

public:
    bool operator== (const PolymorphicBase& s)
    {
        return data_ == s.data_ && inner_data_ == s.inner_data_;
    };

private:
    int inner_data_;
};

SERIALIZATION(SaveLoad, PolymorphicBase)
{
    archive & hierarchy<PolymorphicBaseImpl>(self) & self.inner_data_;
}

class PolymorphicDerived : public PolymorphicBase
{
    SERIALIZABLE(PolymorphicDerived)

public:
    PolymorphicDerived(int data = 0) : PolymorphicBase(data) {}
};

SERIALIZATION(SaveLoad, PolymorphicDerived)
{
    archive & hierarchy<PolymorphicBase>(self);
}

TEST(TestLibrary, TestAccess)
{
    static PolymorphicDerived s_p(123);

    std::vector<unsigned char> storage;
    {
        std::shared_ptr<PolymorphicBase> p = std::make_shared<PolymorphicDerived>(s_p);

        auto ar = oarchive(storage);
        ar & p;
    }
    {
        std::shared_ptr<PolymorphicBase> p = nullptr;

        auto ar = iarchive(storage);
        ar & p;

        ASSERT("non-public inheritance.inited parent", p != nullptr);

        auto d_p = std::dynamic_pointer_cast<PolymorphicDerived>(p);

        ASSERT("non-public inheritance.inited child", d_p != nullptr);
        EXPECT("non-public inheritance.value", *d_p == s_p);
    }
}
//
