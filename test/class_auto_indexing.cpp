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

int main()
{
    auto a = sifar::Access::static_key<some::Struct>(); // 2
    auto b = sifar::Access::static_key<Template<char>>(); // 4
    auto c = sifar::Access::static_key<Class>(); // 1

    std::cout << a << ' ' << b << ' ' << c << '\n';

    return 0;
}
