#include "SFTestingBase.hpp"

#include <SF/BuiltIn/shared_ptr.hpp>

TEST(TestLibrary, TestValidation)
{
    using sf::tracking::track;

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

TEST_SPACE()
{

class Triangle : public instantiable_t
{
    SERIALIZABLE(Triangle)
};

class Cyrcle : public instantiable_t
{
    SERIALIZABLE(Triangle) // oops - polymorphic key is already use
};

// is the same as below
//EXPORT_INSTANTIABLE_KEY("Triangle", Triangle) // good
//EXPORT_INSTANTIABLE_KEY("Triangle", Cyrcle) // bad - same keys

class Square // non instantiable
{
    SERIALIZABLE(Square)
};

} // TEST_SPACE

TEST(TestLibrary, TestInstantiableRegistry)
{
    using sf::dynamic::instantiable_fixture_t;

    {
        instantiable_fixture_t<Triangle> triangle;

        auto success = false;
        try { instantiable_fixture_t<Cyrcle> cyrcle; } catch(...) { success = true; }

        EXPECT("same polymorphic key", success);
    }

    using sf::dynamic::instantiable_registry_t;
    using sf::dynamic::extern_registry_t;

    auto& registry = instantiable_registry_t::instance();
    auto key = __sf::traits<Square>();

    {
        auto success = false;
        try { registry.update<Square>(key); } catch(...) { success = true; }

        EXPECT("non-instantiable type", success);
    }

    {
        auto success = false;
        try { registry.clone<sf::memory::raw_t>(key); } catch(...) { success = true; }

        EXPECT("clone non-instantiable raw", success);
    }
    {
        auto success = false;
        try { registry.clone<sf::memory::shared_t>(key); } catch(...) { success = true; }

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

TEST_SPACE()
{

struct MyStruct : instantiable_t
{
    SERIALIZABLE(MyStruct)
};

class MyClass : sf::instantiable_t // is same as instantiable_t
{
    SERIALIZABLE(MyClass)
};

class MyDerivedClass : public MyClass
{
    SERIALIZABLE(MyDerivedClass)
};

struct MyCustomType : instantiable_t
{
    SERIALIZABLE(MyCustomType)
};

} // TEST_SPACE

EXPORT_INSTANTIABLE_KEY("MyClass", MyStruct)
EXPORT_INSTANTIABLE_KEY("MyStruct", MyClass)

EXPORT_INSTANTIABLE_KEY("MyDerived", MyDerivedClass)

EXPORT_INSTANTIABLE(MyCustomType)

TEST(TestLibrary, TestExportInstantiable)
{
    static auto sv_s = SF_STATIC_HASH("MyClass");
    static auto sv_c = SF_STATIC_HASH("MyStruct");

    {
        EXPECT("export instantiable key.traits",
            __sf::traits<MyStruct>() == sv_s &&
            __sf::traits<MyClass>() == sv_c);

        EXPECT("export instantiable key.static_traits",
            __sf::static_traits<MyStruct>() == sv_c &&
            __sf::static_traits<MyClass>() == sv_s);
    }

    using sf::dynamic::extern_registry_t;

    static auto sv_ct = SF_STATIC_HASH("MyCustomType");

    {
        EXPECT("export instantiable.equivalent",
            __sf::traits<MyCustomType>() == sv_ct &&
            __sf::static_traits<MyCustomType>() == sv_ct);
    }

    static auto sv_dc = SF_STATIC_HASH("MyDerived");

    {
        std::shared_ptr<MyClass> b = std::make_shared<MyDerivedClass>();
        EXPECT("instantiable runtime key.traits", __sf::traits(*b) == sv_dc);
    }
}

#include <SF/BuiltIn/string.hpp>

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
        // or 'sf::memory::const_byte_cast' function
        file.write(storage.data(), storage.size());

        // will close in destructor
        // file.close();
    }
    {
        std::string s;

        std::ifstream file("test.bin", std::ios::binary);

        auto ar = iarchive<sf::wrapper::ifile_stream_t>(file);
        ar & s;

        EXPECT("storage converting", s == s_s);
    }
}

// if you won't serialize object just omit SERIlALIZABLE(type)
// but if you are going to serialize it in the future -
// write SERIALIZABLE(type) and omit SERIALIZATION(mode, type)

TEST_SPACE()
{

struct WorldObject : instantiable_t
{
    SERIALIZABLE(WorldObject)
    int wo = 0;
};

struct EnvironmentObject : virtual WorldObject
{
    SERIALIZABLE(EnvironmentObject)
    int eo = 0;
};

struct MoveableObject : virtual EnvironmentObject
{
    SERIALIZABLE(MoveableObject)
    int mo = 0;
};

struct DestructibleObject : virtual EnvironmentObject
{
    SERIALIZABLE(DestructibleObject)
    int dso = 0;
};

struct DecorativeObject : DestructibleObject, MoveableObject
{
    SERIALIZABLE(DecorativeObject)
    int dco = 0;
};

struct FoliageObject : virtual WorldObject
{
    SERIALIZABLE(FoliageObject)
    int fo = 0;
};

struct FoliageObjectInstance : FoliageObject {};

struct DecorativeFoliageObject : DecorativeObject, FoliageObjectInstance
{
    SERIALIZABLE(DecorativeFoliageObject)
    int dcfo = 0;
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, WorldObject)
{
    ++self.wo;
}

SERIALIZATION(SaveLoad, EnvironmentObject)
{
    ++self.eo;
    archive & hierarchy<WorldObject>(self);
}

SERIALIZATION(SaveLoad, MoveableObject)
{
    ++self.mo;
    archive & hierarchy<EnvironmentObject>(self);
}

SERIALIZATION(SaveLoad, DestructibleObject)
{
    ++self.dso;
    archive & hierarchy<EnvironmentObject>(self);
}

SERIALIZATION(SaveLoad, DecorativeObject)
{
    ++self.dco;
    archive & hierarchy<DestructibleObject, MoveableObject>(self);
}

SERIALIZATION(SaveLoad, FoliageObject)
{
    ++self.fo;
    archive & hierarchy<WorldObject>(self);
}

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

TEST_SPACE()
{

class SomeObjectImpl
{
    SERIALIZABLE(SomeObjectImpl)

public:
    SomeObjectImpl() = default;
    SomeObjectImpl(int data) : data_(data) {}

protected:
    int data_;
};

class SomeObject : protected SomeObjectImpl // try protected inheritance
{
    SERIALIZABLE(SomeObject)

public:
    SomeObject(int data = 0) : SomeObjectImpl(data), inner_data_(data/2) {}

public:
    bool operator== (const SomeObject& s)
    {
        return data_ == s.data_ && inner_data_ == s.inner_data_;
    };

private:
    int inner_data_;
};

struct PolymorphicBaseImpl : public sf::instantiable_t
{
    SERIALIZABLE(PolymorphicBaseImpl)
};

struct PolymorphicBase : protected PolymorphicBaseImpl // not allowed!
{
    SERIALIZABLE(PolymorphicBase)
};

struct PolymorphicDerived : public PolymorphicBase
{
    SERIALIZABLE(PolymorphicDerived)
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, SomeObjectImpl)
{
    archive & self.data_;
}

SERIALIZATION(SaveLoad, SomeObject)
{
    archive & hierarchy<SomeObjectImpl>(self) & self.inner_data_;
}

SERIALIZATION(SaveLoad, PolymorphicBaseImpl) {}

SERIALIZATION(SaveLoad, PolymorphicBase)
{
    archive & hierarchy<PolymorphicBaseImpl>(self);
}

SERIALIZATION(SaveLoad, PolymorphicDerived)
{
    archive & hierarchy<PolymorphicBase>(self);
}

TEST(TestLibrary, TestAccess)
{
    static SomeObject s_so(123);

    std::vector<unsigned char> storage;
    {
        SomeObject so = s_so;

        auto ar = oarchive(storage);
        ar & so;
    }
    {
        SomeObject so;

        auto ar = iarchive(storage);
        ar & so;

        EXPECT("non-public inheritance.value", so == s_so);
    }

    using sf::dynamic::instantiable_registry_t;

    {
        // since PolymorphicBase protected inherited from PolymorphicBaseImpl, where
        // PolymorphicBaseImpl is instantiable type - we cannot use dynamic_cast for serialization
        // since we get nullptr after apply casting

        EXPECT("public instantiable", instantiable_registry_t::is_instantiable<PolymorphicBaseImpl>::value);

        EXPECT("non-public instantiable",
            !instantiable_registry_t::is_instantiable<PolymorphicBase>::value &&
            !instantiable_registry_t::is_instantiable<PolymorphicDerived>::value);
    }
}

// since we dont use SERIALIZABLE remember that:
// SERIALIZABLE contains SERIALIZATION_ACCESS() and INSTANTIABLE_BODY() macros

TEST_SPACE()
{

struct NoTraitsBase : sf::instantiable_t
{
    int b = 0;
};

struct NoTraitsDerived : NoTraitsBase
{
    int d = 0;
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, NoTraitsBase)
{
    archive & self.b;
}

SERIALIZATION(SaveLoad, NoTraitsDerived)
{
    archive & hierarchy<NoTraitsBase>(self) & self.d;
}

// if user does not use SERIALIZABLE macro (which include SERIALIZATION_TRAITS),
// then library will use typeid for type hashing,
// mixed usage SERIALIZATION_TRAITS and typeid does not allowed (in single hierarchy only)!
// You also can specify hashing behavior with macro SF_TYPE_HASH - see Core/Hash.hpp
// Note that: EXPORT_INSTANTIABLE_KEY("some_text", some_type) - is not allowed with typeid using
// SERIALIZABLE macro consists of SERIALIZATION_ACCESS, SERIALIZATION_FIXTURE and SERIALIZATION_TRAITS

TEST(TestLibrary, TestNoTraits)
{
    using sf::serializable;

    static NoTraitsDerived s_d;
    s_d.b = 246253;
    s_d.d = 4895792;

    std::vector<unsigned char> storage;
    {
        serializable<NoTraitsDerived>();

        std::shared_ptr<NoTraitsBase> b = std::make_shared<NoTraitsDerived>(s_d);

        auto ar = oarchive(storage);
        ar & b;
    }
    {
        std::shared_ptr<NoTraitsBase> b = nullptr;

        auto ar = iarchive(storage);
        ar & b;

        auto d = std::dynamic_pointer_cast<NoTraitsDerived>(b);

        ASSERT("no-traits.inited", d != nullptr);
        EXPECT("no-traits.value", d->b == s_d.b && d->d == s_d.d);
    }
}

#include "TestLibrary/BaseObject.hpp"
#include "TestLibrary/DerivedObject.hpp"

TEST(TestLibrary, TestPartition)
{
    static DerivedObject s_d;
    s_d.id = 1894981;
    s_d.name = "Unit";
    s_d.data = 3.14f;

    std::vector<unsigned char> storage;
    {
        std::shared_ptr<BaseObject> b = std::make_shared<DerivedObject>(s_d);

        auto ar = oarchive(storage);
        ar & b;
    }
    {
        std::shared_ptr<BaseObject> b = nullptr;

        auto ar = iarchive(storage);
        ar & b;

        auto d = std::dynamic_pointer_cast<DerivedObject>(b);

        ASSERT("saveload partition.inited", d != nullptr);
        EXPECT("saveload partition.value",
            d->id == s_d.id && d->name == s_d.name && d->data == s_d.data);
    }
}

#if __cplusplus >= 201703L

TEST_SPACE()
{

struct AggregateObject
{
    int i;
    float f;
    char c;
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, AggregateObject)
{
    archive & self.i & self.f; // ignoring self.c
}

TEST(TestLibrary, TestAggregateOverload)
{
    using sf::aggregate;

    static AggregateObject s_nao { 123, 3.14f, 'F' };
    static AggregateObject s_ao { 8938, 2.17f, 's' };

    std::vector<unsigned char> storage;
    {
        AggregateObject nao = s_nao;
        AggregateObject ao = s_ao;

        auto ar = oarchive(storage);
        ar & nao // will serialize as user type by __sf::SaveLoad
           & aggregate(ao); // will serialize as aggregate type
    }
    {
        AggregateObject nao {};
        AggregateObject ao {};

        auto ar = iarchive(storage);
        ar & nao
           & aggregate(ao);

        EXPECT("serialization as non-aggregate.", nao.i == s_nao.i && nao.f == s_nao.f && nao.c != s_nao.c);
        EXPECT("serialization as aggregate.", ao.i == s_ao.i && ao.f == s_ao.f && ao.c == s_ao.c);
    }
}

#endif // if

#include <SF/BuiltIn/unique_ptr.hpp>

TEST_SPACE()
{

struct Interface : sf::instantiable_t
{
    SERIALIZABLE(Interface)

    virtual void logic() = 0;
};

struct Implementation : Interface
{
    SERIALIZABLE(Implementation)

    virtual void logic() override { /*some logic...*/ }
};

} // TEST_SPACE

SERIALIZATION(SaveLoad, Interface) {}
SERIALIZATION(SaveLoad, Implementation) {}

// or general solution
// CONDITIONAL_SERIALIZATION(SaveLoad, std::is_base_of<Interface, T>::value) {}

TEST(TestLibrary, TestAbstract)
{
    std::vector<unsigned char> storage;
    {
        std::unique_ptr<Interface> i(new Implementation);

        auto ar = oarchive(storage);
        ar & i;
    }
    {
        std::unique_ptr<Interface> i;

        auto ar = iarchive(storage);
        ar & i;

        ASSERT("inited", i != nullptr);
        EXPECT("traits", __sf::traits(*i) == __sf::traits<Implementation>());
    }
}

TEST_SPACE()
{

struct NoMacroObject
{
    std::string data;
};

// since we want to serialize object using pointer to base type,
// we should inherit from instantiable type, which defined by macro INSTANTIABLE_TYPE
// Note that: any derived type from instantiable becomes polymorphic,
// becose instantiable type has virtual destructor (requrement of sf library)
struct NoMacroBase : sf::instantiable_t
{
    int b;
};

struct NoMacroDerived : NoMacroBase
{
    int d;
};

} // TEST_SPACE

// extern serialization - useful for open classes
template <class Archive>
Archive& operator& (Archive& archive, NoMacroObject& self)
{
    archive & self.data;
    return archive;
}

// inner serialization - useful for open/close class attributes (standard serialization)
template <>
struct __sf::SaveLoad<NoMacroDerived>
{
    template <class Archive>
    SaveLoad(Archive& archive, NoMacroDerived& self)
    {
        archive & sf::hierarchy<NoMacroBase>(self) & self.d;
    }
};

// inner serialization with split
// polymorphic archive - useful for hide impl to translation unit
template <>
struct __sf::Save<NoMacroBase>
{
    Save(sf::core::ioarchive_t& archive, NoMacroBase& self)
    {
        archive << self.b;
    }
};

// for non polymorphic archive we can use operator&, and not only operator>>
template <>
struct __sf::Load<NoMacroBase>
{
    template <class Archive>
    Load(Archive& archive, NoMacroBase& self)
    {
        archive & self.b; // or archive >> self.b
    }
};

TEST(TestLibrary, TestNoMacro)
{
    static std::string s_data = "abc\"def;";

    static int s_b = 213;
    static int s_d = 32413;

    std::vector<unsigned char> storage;
    {
        sf::serializable<NoMacroDerived>(); // registry

        NoMacroObject nmo;
        nmo.data = s_data;

        NoMacroDerived* nmd = new NoMacroDerived;
        nmd->b = s_b;
        nmd->d = s_d;

        NoMacroBase* nmb = nmd;

        auto ar = oarchive(storage);
        ar & nmo & nmb;
    }
    {
        NoMacroObject nmo;
        NoMacroBase* nmb = nullptr;

        auto ar = iarchive(storage);
        ar & nmo & nmb;

        EXPECT("no-macro.value", nmo.data == s_data);

        auto nmd = dynamic_cast<NoMacroDerived*>(nmb);
        ASSERT("no-macro polymorphic.inited", nmd != nullptr);

        EXPECT("no-macro polymorphic.value", nmd->b == s_b && nmd->d == s_d);
    }
}

TEST(TestLibrary, Testpolymorphic_archive_t)
{
    static std::string s_data = "Hello, SF!";

    std::vector<unsigned char> storage;
    {
        std::string data = s_data;

        auto oar = oarchive(storage);
        sf::core::ioarchive_t& ar = oar;
        ar << data;
    }
    {
        std::string data;

        sf::core::ioarchive_t&& ar = iarchive(storage);
        ar >> data;

        EXPECT("value", data == s_data);
    }

    storage.clear();
    {
        int some_data = 0;

        sf::core::ioarchive_t&& ar = oarchive(storage);

        auto success = false;
        try { ar >> some_data; } catch(...) { success = true; }

        EXPECT("bad oarchive", success);

        ar << some_data;
    }
    {
        int some_data = 0;

        sf::core::ioarchive_t&& ar = iarchive(storage);

        auto success = false;
        try { ar << some_data; } catch(...) { success = true; }

        EXPECT("bad iarchive", success);
    }
}
