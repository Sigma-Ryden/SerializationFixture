# Eightser - Simple serialization library

`Eightser` is a flexible and extensible C++ library with many features, that will allow you to easily
and quickly turn your objects into a sequence of bits.
The library has a small code base, but enough to use it.

Use macros for code generation or read the documentation and write your own code.
There are 2 main serialization classes under your control: `oarchive_t` and `iarchive_t`.
Both classes can be equipped with additional helper classes via template parameters if needed.
A wrapper for streams will help you flexibly configure the process of writing and reading data.
The type registrar will allow you to properly serialize and deserialize polymorphic and abstract types.

A library implemented purely in C++11 (until v2.4.0) and C++17 (since v2.5.0).

See last stable library version [here](https://github.com/Sigma-Ryden/Eightser/releases).

## Functional:

### Serialization of common types:
- All arithmetic types
- Enum or scoped enum
- Static or dynamic array
- Bit fields

### Data tracking:
- Raw pointers
- Smart pointers: unique, shared, weak
- Aliasing as referencing

### Serialization of standard library types:
- C++11: string, vector, map, and many others
- C++17: any, optional, variant

See full list [here](https://github.com/Sigma-Ryden/Eightser/tree/master/include/Eightser/BuiltIn).
### Serialization of user classes:
- Aggregate (since C++17)
- Common/Union/Template
- Hierarchy
- Polymorphic/Abstract
- Virtual Inheritance

## Auto Tests:
See library testing [here](https://github.com/Sigma-Ryden/Eightser/tree/master/test).

## Quick start:
Warning! This is a simple example. You can see more examples [here](https://github.com/Sigma-Ryden/Eightser/tree/master/test/src).

Preparing:

```C++
#include <Eightser/Core.hpp>
```
Let's include serialization support of common STL types:
```C++
#include <Eightser/BuiltIn/string.hpp>
#include <Eightser/BuiltIn/vector.hpp>
#include <Eightser/BuiltIn/map.hpp>
#include <Eightser/BuiltIn/shared_ptr.hpp>
```
And let's write own serializable types:
```C++
enum class Property
{
    Speed,
    Force,
    inteligance,
    Fly
};

struct Prototype
{
    std::string name;
    std::vector<Property> properties;
};

struct Handbook
{
    std::map<int, std::shared_ptr<Prototype>> prototypes;
};
```
Since C++17 we can ommit it, for aggregate types:
```C++
// in .hpp files
SERIALIZABLE_DECLARATION(Prototype)
SERIALIZABLE_DECLARATION_INIT()

SERIALIZABLE_DECLARATION(Handbook)
SERIALIZABLE_DECLARATION_INIT()
// ~

// in .cpp files
SERIALIZABLE(saveload, self, Prototype)
    SERIALIZATION
    (
        archive & self.name & self.properties;
    )
SERIALIZABLE_INIT()

SERIALIZABLE(saveload, self, Handbook)
    SERIALIZATION
    (
        archive & self.prototypes;
    )
SERIALIZABLE_INIT()
// ~
```
Explaining of using macros above:
- ```SERIALIZABLE_DECLARATION(<type>) <properties> SERIALIZABLE_DECLARATION_INIT()``` - Generate header code to access the implementation for the specified class.
- ```SERIALIZABLE(<mode>, <object>, <type>) <serialization> SERIALIZABLE_INIT()``` - Generate 'save/load/saveload' serialization functions for the specified class.
- ```SERIALIZATION(<body>)``` - Generate 'if' branch for 'bin/xml/json' and custom archive types (bin is the default).

### Using of serialization library:

Sinse we are going to use memory storage, let's define a variable for it:
```C++
std::vector<unsigned char> storage;
```
Let's prepare our data for save:
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
ifly->properties = {Property::inteligance, Property::Fly};

db.prototypes[0] = zero;
db.prototypes[3] = rew;
db.prototypes[2] = ifly;
```

Serialization prepared data:
```C++
auto ar = eightser::oarchive(storage);
ar & db;
```

Deserialization from storage:
```C++
Handbook db; // some other clear db

auto ar = eightser::iarchive(storage);
ar & db;
```
See full code here: [TestExample.cpp](https://github.com/Sigma-Ryden/Eightser/tree/master/test/src/TestExample.cpp)

### Notes:
For ```oarchive_t``` objects, you may also using overloaded ```operator <<``` instead ```operator &```
or ```operator()```, and also ```operator >>``` for ```iarchive_t``` objects.

Examples:
```C++
// saving objects to the archive
archive << obj_0 << ... << obj_n;
// loading objects from the archive
archive >> obj_0 >> ... >> obj_n;

//depends on archive type
archive & obj_0 & ... & obj_n;
// or
archive(obj_0, ..., obj_n);
```
## License:
This library is an open source project licensed under: [MIT](https://opensource.org/licenses/MIT).
