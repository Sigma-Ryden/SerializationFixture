# Simple serialization library

A flexible and extensible C++ library with many features, that will allow you to easily
and quickly turn your objects into a sequence of bits.
The library has a small code base, but enough to use it.

Use macros for code generation or read the documentation and write your own code.
There are 2 main serialization classes under your control: WriteArchive and ReadArchive.
Both classes can be equipped with additional helper classes via template parameters if needed.
The type Registrar will allow you to properly serialize and deserialize polymorphic and abstract types.
A wrapper for streams will help you flexibly configure the process of writing and reading data.

Is a header-only library implemented purely in C++11.

# Functional:

#### Serialization of common types:
- arithmetic
- enum
- static array
- pointer
#### Sserialization of library types:
- STL: string, vector, array...
- any usage type
#### serialization of users class:
- common
- hierarchy
- polymorphic
- abstract

### Quick start.

Preparing:

```C++
#include "Serialization/Core.hpp"
```
Let's include support of std::string serialization:
```C++
#include "Serialization/Support/string.hpp"

using namespace serialization::library; // support of string

using serialization::WriteArchive;
using serialization::ReadArchive;
```
And let's equip our simple class with serialization support:
```C++
class Shape
{
    SERIALIZATION_ARCHIVE_ACCESS()

private:
    std::string name_;
    float x_, y_;

public:
    Shape() : name_("UNNAMED"), x_(0), y_(0) {}

    Shape(const std::string& name, float x,  float y)
        : name_(name), x_(x), y_(y) {}

    void print() const
    {
        std::cout << name_ << " shape is lacated at: ("
                  << x_ << ", " << y_ << ")\n";
    }

private:
    SERIALIZATION_UNIFIED(ar)
    {
        ar & name_;
        ar & x_;
        ar & y_;

        return ar;
    }
};
```
Explaining of using macros above:
- ```SERIALIZATION_ARCHIVE_ACCESS()``` - Provide us with secure saving and loading of objects.
You can omit this macro if the serialization functions are public.
- ```SERIALIZATION_UNIFIED()``` - Generate save/load serialization functions for given class.
This macro allows you to split into two separate macros: ```SERIALIZATION_SAVE()``` and ```SERIALIZATION_LOAD()``` if needed.

### Using of serialization library:

Sinse we are going to use hard drive storage, let's include standard file stream:
```C++
#include <fstream> // ifstream, ofstream
#include <iostream> // cout
```
```C++
void save(Shape& shape)
{
    std::ofstream file("example.bin", std::ios::binary);

    if (not file.is_open()) return;

    WriteArchive<std::ofstream> ar(file);

    ar & shape;

    file.close();
}
```
```C++
void load(Shape& shape)
{
    std::ifstream file("example.bin", std::ios::binary);

    if (not file.is_open()) return;

    ReadArchive<std::ifstream> ar(file);

    ar & shape;

    file.close();
}
```
Processing:
```C++
int main()
{
    // Saving of Shape object
    {
        Shape shape("Square", 0.1234f, 5.6789f);
        save(shape);
    }
    // Loading of Shape object
    {
        Shape shape;
        shape.print();

        load(shape);

        shape.print();
    }

    return 0;
}
```
### Output:
```console
UNNAMED shape is lacated at: (0, 0)
Square shape is lacated at: (0.1234, 5.6789)
```
#### Notes:
In this case the ```load``` and ```save``` functions are just a division of the code into blocks.
It does not affect the library's performance in any way.

For ```WriteArchive``` objects, you may also using overloaded ```operator <<``` instead ```operator &```,
and also ```operator >>``` for ```ReadArchive``` objects.

Examples:
```C++
// saving objects to the archive
archive << shape << vector << animal << human;
// loading objects from the archive
archive >> shape >> vector >> animal >> human;
//depends on archive type
archive & shape & vector & animal & human;
```

# License:
This library is an open source project licensed under: [MIT](https://opensource.org/licenses/MIT).
