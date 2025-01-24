#include <SFTestingBase.hpp>

#include <SerializationFixture/BuiltIn/shared_ptr.hpp>

TEST(TestLibrary, TestValidation)
{
    using sf::track;

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

class Triangle : public instantiable_t {};
class Cyrcle : public instantiable_t {};

// non instantiable
class Square {};

} // TEST_SPACE

EXPORT_INSTANTIABLE_KEY(SF_TYPE_HASH(Cyrcle), Triangle) // export Triangle as Cyrcle - will collide

SERIALIZABLE_DECLARATION(Triangle)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(Cyrcle)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(Square)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, Triangle)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, Cyrcle)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, Square)
    SERIALIZATION()
SERIALIZABLE_INIT()

TEST(TestLibrary, TestInstantiableRegistry)
{
    using sf::dynamic::instantiable_registry;

    {
        auto success = false;
        try { instantiable_registry.fixture<Cyrcle>(); } catch(...) { success = true; }

        EXPECT("same polymorphic key", success);
    }

    {
        auto success = false;
        try { instantiable_registry.fixture<Square>(); } catch(...) { success = true; }

        EXPECT("non-instantiable type", success);
    }

    const auto key = ::xxsf_instantiable_traits<Square>::key();

    {
        auto success = false;
        try { Square* r = nullptr; instantiable_registry.clone(r, key); } catch(...) { success = true; }

        EXPECT("clone non-instantiable raw", success);
    }
    {
        auto success = false;
        try { std::shared_ptr<Square> s = nullptr; instantiable_registry.clone(s, key); } catch(...) { success = true; }

        EXPECT("clone non-instantiable shared", success);
    }

    {
        auto r = new Square;

        auto success = false;
        try { void* address = nullptr; (void)instantiable_registry.cast(r, address, key); } catch(...) { success = true; }

        EXPECT("cast non-instantiable raw", success);

        delete r; // clean up
    }
    {
        auto s = std::make_shared<Square>();

        auto success = false;
        try { std::shared_ptr<void> address = nullptr; (void)instantiable_registry.cast(s, address, key); } catch(...) { success = true; }

        EXPECT("cast non-instantiable shared", success);
    }
}

TEST_SPACE()
{

struct MyStruct : instantiable_t {};

// is same as instantiable_t
class MyClass : public sf::instantiable_t {};

class MyDerivedClass : public MyClass {};

struct MyCustomType : instantiable_t {};

} // TEST_SPACE

EXPORT_INSTANTIABLE_KEY(SF_STRING_HASH("MyClass"), MyStruct)
EXPORT_INSTANTIABLE_KEY(SF_STRING_HASH("MyStruct"), MyClass)

EXPORT_INSTANTIABLE_KEY(SF_STRING_HASH("MyDerived"), MyDerivedClass)

EXPORT_INSTANTIABLE(MyCustomType)

SERIALIZABLE_DECLARATION(MyStruct)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, MyStruct)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(MyClass)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, MyClass)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(MyDerivedClass)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, MyDerivedClass)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(MyCustomType)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, MyCustomType)
    SERIALIZATION()
SERIALIZABLE_INIT()

TEST(TestLibrary, TestExportInstantiable)
{
    using sf::dynamic::instantiable_registry;

    static auto sv_s = SF_STRING_HASH("MyClass");
    static auto sv_c = SF_STRING_HASH("MyStruct");

    {
        EXPECT("export instantiable key.traits",
            ::xxsf_instantiable_traits<MyStruct>::key() == sv_s &&
            ::xxsf_instantiable_traits<MyClass>::key() == sv_c);
    }

    static auto sv_ct = SF_TYPE_HASH(MyCustomType);

    {
        EXPECT("export instantiable.equivalent", ::xxsf_instantiable_traits<MyCustomType>::key() == sv_ct);
    }

    static auto sv_dc = SF_STRING_HASH("MyDerived");

    {
        std::shared_ptr<MyClass> b = std::make_shared<MyDerivedClass>();
        auto& rb = *b;

        EXPECT("instantiable runtime key.traits", instantiable_registry.rtti_all.at(SF_EXPRESSION_HASH(rb)).key == sv_dc);
    }
}

#include <SerializationFixture/BuiltIn/string.hpp>

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
// write SERIALIZABLE(type) and omit SERIALIZABLE(mode, type)

TEST_SPACE()
{

struct WorldObject : instantiable_t
{
    int wo = 0;
};

struct EnvironmentObject : virtual WorldObject
{
    int eo = 0;
};

struct MoveableObject : virtual EnvironmentObject
{
    int mo = 0;
};

struct DestructibleObject : virtual EnvironmentObject
{
    int dso = 0;
};

struct DecorativeObject : DestructibleObject, MoveableObject
{
    int dco = 0;
};

struct FoliageObject : virtual WorldObject
{
    int fo = 0;
};

struct FoliageObjectInstance : FoliageObject {};

struct DecorativeFoliageObject : DecorativeObject, FoliageObjectInstance
{
    int dcfo = 0;
};

} // TEST_SPACE

EXPORT_INSTANTIABLE(WorldObject)
EXPORT_INSTANTIABLE(EnvironmentObject)
EXPORT_INSTANTIABLE(MoveableObject)
EXPORT_INSTANTIABLE(DestructibleObject)
EXPORT_INSTANTIABLE(DecorativeObject)
EXPORT_INSTANTIABLE(FoliageObject)
EXPORT_INSTANTIABLE(DecorativeFoliageObject)

SERIALIZABLE_DECLARATION(WorldObject)
    //INSTANTIABLE(WorldObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(EnvironmentObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(MoveableObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(DestructibleObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(DecorativeObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(FoliageObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(DecorativeFoliageObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, WorldObject)
    SERIALIZATION
    (
        ++self.wo;
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, EnvironmentObject)
    SERIALIZATION
    (
        ++self.eo;
        archive & hierarchy<WorldObject>(self);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, MoveableObject)
    SERIALIZATION
    (
        ++self.mo;
        archive & hierarchy<EnvironmentObject>(self);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, DestructibleObject)
    SERIALIZATION
    (
        ++self.dso;
        archive & hierarchy<EnvironmentObject>(self);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, DecorativeObject)
    SERIALIZATION
    (
        ++self.dco;
        archive & hierarchy<DestructibleObject, MoveableObject>(self);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, FoliageObject)
    SERIALIZATION
    (
        ++self.fo;
        archive & hierarchy<WorldObject>(self);
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, DecorativeFoliageObject)
    SERIALIZATION
    (
        ++self.dcfo;
        archive & hierarchy<DecorativeObject, FoliageObject>(self);
    )
SERIALIZABLE_INIT()

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
    SERIALIZABLE_ACCESS()

public:
    SomeObjectImpl() = default;
    SomeObjectImpl(int data) : data_(data) {}

protected:
    int data_;
};

class SomeObject : protected SomeObjectImpl
{
    SERIALIZABLE_ACCESS()

public:
    SomeObject(int data = 0) : SomeObjectImpl(data), inner_data_(data/2) {}

public:
    bool operator== (const SomeObject& s)
    {
        return data_ == s.data_ && inner_data_ == s.inner_data_;
    }

private:
    int inner_data_;
};

struct PolymorphicBaseImpl : public sf::instantiable_t {};
struct PolymorphicBase : protected PolymorphicBaseImpl {}; // not allowed!
struct PolymorphicDerived : public PolymorphicBase {};

} // TEST_SPACE

// will throw expection
//EXPORT_INSTANTIABLE(SomeObjectImpl)
//EXPORT_INSTANTIABLE(SomeObject)
//EXPORT_INSTANTIABLE(PolymorphicDerived)
//EXPORT_INSTANTIABLE(PolymorphicBase)
//EXPORT_INSTANTIABLE(PolymorphicBaseImpl)

SERIALIZABLE_DECLARATION(SomeObjectImpl)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, SomeObjectImpl)
    SERIALIZATION
    (
        archive & self.data_;
    )
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(SomeObject)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, SomeObject)
    SERIALIZATION
    (
        archive & hierarchy<SomeObjectImpl>(self) & self.inner_data_;
    )
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(PolymorphicBaseImpl)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, PolymorphicBaseImpl)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(PolymorphicBase)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, PolymorphicBase)
    SERIALIZATION
    (
        // not compile for: non-public inheritance
        // archive & hierarchy<PolymorphicBaseImpl>(self);
    )
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(PolymorphicDerived)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, PolymorphicDerived)
    SERIALIZATION
    (
        archive & hierarchy<PolymorphicBase>(self);
    )
SERIALIZABLE_INIT()

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

    using instantiable_registry_type = sf::dynamic::instantiable_registry_t<INSTANTIABLE_VOID_POINTER_TYPES>;

    {
        // since PolymorphicBase protected inherited from PolymorphicBaseImpl, where
        // PolymorphicBaseImpl is instantiable type - we cannot use dynamic_cast for serialization
        // since we get nullptr after apply casting

        EXPECT("public instantiable", instantiable_registry_type::is_instantiable<PolymorphicBaseImpl>::value);

        EXPECT("non-public instantiable",
            !instantiable_registry_type::is_instantiable<PolymorphicBase>::value &&
            !instantiable_registry_type::is_instantiable<PolymorphicDerived>::value);
    }
}

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

SERIALIZABLE_DECLARATION(NoTraitsBase)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, NoTraitsBase)
    SERIALIZATION
    (
        archive & self.b;
    )
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(NoTraitsDerived)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, NoTraitsDerived)
    SERIALIZATION
    (
        archive & hierarchy<NoTraitsBase>(self) & self.d;
    )
SERIALIZABLE_INIT()

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

SERIALIZABLE_DECLARATION(AggregateObject)
SERIALIZABLE_DECLARATION_INIT()

// it's works only for full template specialization
SERIALIZABLE(saveload, self, AggregateObject)
    SERIALIZATION
    (
        archive & self.i & self.f; // ignoring self.c
    )
SERIALIZABLE_INIT()

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
        ar & nao // will serialize as user type by ::xxsfsaveloadd
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

#include <SerializationFixture/BuiltIn/unique_ptr.hpp>

TEST_SPACE()
{

struct Interface : sf::instantiable_t
{
    virtual void logic() = 0;
};

struct Implementation : Interface
{
    virtual void logic() override { /*some logic...*/ }
};

} // TEST_SPACE

EXPORT_INSTANTIABLE(Interface)
EXPORT_INSTANTIABLE(Implementation)

SERIALIZABLE_DECLARATION(Interface)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, Interface)
    SERIALIZATION()
SERIALIZABLE_INIT()

SERIALIZABLE_DECLARATION(Implementation)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE(saveload, self, Implementation)
    SERIALIZATION()
SERIALIZABLE_INIT()

// or general solution
// CONDITIONAL_SERIALIZABLE(saveload, std::is_base_of<Interface, T>::value) {}

TEST(TestLibrary, TestAbstract)
{
    using sf::dynamic::instantiable_registry;

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

        auto& ri = *i;

        const auto hash = SF_EXPRESSION_HASH(ri);
        const auto key = ::xxsf_instantiable_traits<Implementation>::key();

        EXPECT("traits", instantiable_registry.rtti_all.at(hash).key == instantiable_registry.all.at(key).key);
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
template <class ArchiveType>
ArchiveType& operator& (ArchiveType& archive, NoMacroObject& self)
{
    archive & self.data;
    return archive;
}

// inner serialization - useful for open/close class attributes (standard serialization)
template <>
struct xxsf<NoMacroDerived>
{
    template <class ArchiveType>
    static void save(ArchiveType& archive, NoMacroDerived& self)
    {
        archive & sf::hierarchy<NoMacroBase>(self) & self.d;
    }

    template <class ArchiveType>
    static void load(ArchiveType& archive, NoMacroDerived& self)
    {
        archive & sf::hierarchy<NoMacroBase>(self) & self.d;
    }
};

// inner serialization with split
// polymorphic archive - useful for hide impl to translation unit
template <>
struct xxsf<NoMacroBase>
{
    static void save(sf::ioarchive_t& archive, NoMacroBase& self)
    {
        archive << self.b;
    }

    // for non polymorphic archive we can use operator&, and not only operator>>
    template <class ArchiveType>
    static void load(ArchiveType& archive, NoMacroBase& self)
    {
        archive & self.b; // or archive >> self.b
    }
};

template <>
struct xxsf_instantiable_traits<NoMacroDerived>
{
    static ::xxsf_instantiable_traits_key_type key()
    {
        return SF_STRING_HASH("NoMacroDerived");
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

TEST(TestLibrary, TestPolymorphicArchive)
{
    static std::string s_data = "Hello, SF!";

    std::vector<unsigned char> storage;
    {
        std::string data = s_data;

        auto oar = oarchive(storage);
        sf::ioarchive_t& ar = oar;
        ar << data;
    }
    {
        std::string data;

        sf::ioarchive_t&& ar = iarchive(storage);
        ar >> data;

        EXPECT("value", data == s_data);
    }

    storage.clear();
    {
        int some_data = 0;

        sf::ioarchive_t&& ar = oarchive(storage);

        auto success = false;
        try { ar >> some_data; } catch(...) { success = true; }

        EXPECT("bad oarchive", success);

        ar << some_data;
    }
    {
        int some_data = 0;

        sf::ioarchive_t&& ar = iarchive(storage);

        auto success = false;
        try { ar << some_data; } catch(...) { success = true; }

        EXPECT("bad iarchive", success);
    }
}
