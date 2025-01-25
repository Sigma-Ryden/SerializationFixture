# SF (SerializationFixture) - Simple serialization library

`SF` is a flexible and extensible C++ library with many features, that will allow you to easily
and quickly turn your objects into a sequence of bits.
The library has a small code base, but enough to use it.

Use macros for code generation or read the documentation and write your own code.
There are 2 main serialization classes under your control: `oarchive_t` and `iarchive_t`.
Both classes can be equipped with additional helper classes via template parameters if needed.
A wrapper for streams will help you flexibly configure the process of writing and reading data.
The type registrar will allow you to properly serialize and deserialize polymorphic and abstract types.

A library implemented purely in C++11.

See last stable library version [here](https://github.com/Sigma-Ryden/SerializationFixture/releases).

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

See full list [here](https://github.com/Sigma-Ryden/SerializationFixture/tree/master/include/SF/BuiltIn).
### Serialization of user classes:
- Aggregate (since C++17)
- Common/Union/Template
- Hierarchy
- Polymorphic/Abstract
- Virtual Inheritance

## Auto Tests:
See library testing [here](https://github.com/Sigma-Ryden/SerializationFixture/tree/master/test).

## License:
This library is an open source project licensed under: [MIT](https://opensource.org/licenses/MIT).