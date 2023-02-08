# Siraf - Simple serialization library

`Siraf` is a flexible and extensible C++ library with many features, that will allow you to easily
and quickly turn your objects into a sequence of bits.
The library has a small code base, but enough to use it.

Use macros for code generation or read the documentation and write your own code.
There are 2 main serialization classes under your control: `WriteArchive` and `ReadArchive`.
Both classes can be equipped with additional helper classes via template parameters if needed.
A wrapper for streams will help you flexibly configure the process of writing and reading data.
The type registrar will allow you to properly serialize and deserialize polymorphic and abstract types.

Is a header-only library implemented purely in C++11.

# Functional:

#### Serialization of common types:
- all arithmetic types
- enum or scoped enum
- static or dynamic array
- raw & smart pointers (with data tracking)
#### Sserialization of library types:
- STL: string, vector, array...
- any usage types
#### serialization of users class:
- common
- hierarchy
- polymorphic
- abstract
- template

### Quick start.
Warning! This is a simple example. You can see real applied examples [here](https://github.com/Sigma-Ryden/Siraf/tree/master/test).

Preparing:

```C++
#include <Siraf/Core.hpp>
```
Let's include serialization support of common STL types:
```C++
#include <Siraf/Support/string.hpp>
#include <Siraf/Support/vector.hpp>
#include <Siraf/Support/map.hpp>
#include <Siraf/Support/shared_ptr.hpp>
```
And let's write own serializable types:
```C++
enum class Property
{
    Speed,
    Force,
    Intelige,
    Fly
};

struct Prototype
{
    SERIALIZABLE(Prototype)

    std::string name;
    std::vector<Property> properties;
};

struct Handbook
{
    SERIALIZABLE(Handbook)

    std::map<int, std::shared_ptr<Prototype>> prototypes;
};

SERIALIZATION(SaveLoad, Prototype)
{
    archive & self.name & self.properties;
}

SERIALIZATION(SaveLoad, Handbook)
{
    archive & self.prototypes;
}
```
Explaining of using macros above:
- ```SERIALIZABLE(<type>)``` - Provide us with secure saving and loading of objects.
- ```SERIALIZATION(<mode>, <type>)``` - Generate Save/Load/SaveLoad serialization functions for given class.

### Using of serialization library:

Sinse we are going to use memory storage, let's define a variable for it:
```C++
std::vector<unsigned char> storage;
```
Preparing our data:
```C++
Handbook db;

auto zero = std::make_shared<Prototype>();
zero->name = "Zero";
zero->properties = {Property::Speed};

auto rew = std::make_shared<Prototype>();
rew->name = "Rew";
rew->properties = {Property::Force};

auto ifly = std::make_shared<Prototype>();
ifly->name = "I.Fly";
ifly->properties = {Property::Intelige, Property::Fly};

db.prototypes[0] = zero;
db.prototypes[3] = rew;
db.prototypes[2] = ifly;
```

Serialization:
```C++
using siraf::oarchive;
using siraf::wrapper::OByteStream;

auto ar = oarchive<OByteStream>(storage);
ar & db;
```

Deserialization:
```C++
using siraf::iarchive;
using siraf::wrapper::IByteStream;

Handbook db;

auto ar = iarchive<IByteStream>(storage);
ar & db;
```
See full code here: [example](https://github.com/Sigma-Ryden/Siraf/tree/master/test/example.cpp)

#### Notes:
In this case the ```load``` and ```save``` functions are just a division of the code into blocks.
It does not affect the library's performance in any way.

For ```WriteArchive``` objects, you may also using overloaded ```operator <<``` instead ```operator &```,
and also ```operator >>``` for ```ReadArchive``` objects.

Examples:
```
// saving objects to the archive
archive << shape << vector << animal << human;
// loading objects from the archive
archive >> shape >> vector >> animal >> human;

//depends on archive type
archive & shape & vector & animal & human;

// or operator()
archive(shape, vector, animal, human);
```

# License:
This library is an open source project licensed under: [MIT](https://opensource.org/licenses/MIT).
