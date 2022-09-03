#include <fstream> // ifstream, ofstream
#include <iostream> // cout

#define SIFAR_EXPERIMENTAL
#include <Sifar/Core.hpp> // Access

class Class
{
    SERIALIZATION_ACCESS()
    SERIALIZATION_POLYMORPHIC()

    virtual ~Class() {}
};

SERIALIZATION_EXPORT(Class)

namespace some {
struct Struct
{
    SERIALIZATION_POLYMORPHIC()

    virtual ~Struct() {}
};
} // namespace some

SERIALIZATION_EXPORT(some::Struct)

template <class T>
struct Template
{
    SERIALIZATION_POLYMORPHIC()

    virtual ~Template() {}
};

SERIALIZATION_EXPORT_TPL(Template<int>)
SERIALIZATION_EXPORT_TPL(Template<char>)

struct A
{
    int a_;
    A() : a_(0) { std::cout << "A()\n"; }
    A(int a) : a_(a) {std::cout << "A(int)\n"; }
    A(A&& a) : a_(std::move(a.a_)) {std::cout << "A(A&&)\n"; }
    A(const A& a) : a_(a.a_) {std::cout << "A(const A&)\n"; }

    ~A() { std::cout << "~A()\n"; }
};

SERIALIZATION_SAVE_DATA(obj, std::is_same<T, A>::value)
{
    archive & obj.a_;

    return archive;
}

SERIALIZATION_LOAD_DATA(obj, std::is_same<T, A>::value)
{
    archive & obj.a_;

    return archive;
}

SERIALIZATION_TYPE_REGISTRY(A)

int main()
{
    auto a = sifar::Access::static_key<some::Struct>(); // 2
    auto b = sifar::Access::static_key<Template<char>>(); // 4
    auto c = sifar::Access::static_key<Class>(); // 1

    std::cout << a << ' ' << b << ' ' << c << '\n';

    return 0;
}
