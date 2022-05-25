cmake_minimum_required(VERSION 3.5)

project(Serializer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} test.cpp
    Serialization/Detail/Tools.hpp
    Serialization/Detail/Pointer.hpp
    Serialization/Detail/Meta.hpp
    Serialization/Detail/MacroScope.hpp
    Serialization/Detail/MacroUnscope.hpp
    Serialization/Support/vector.hpp
    Serialization/Support/array.hpp
    Serialization/Support/string.hpp
    Serialization/UndefMacro.hpp
    Serialization/Hash.hpp
    Serialization/Scope.hpp
    Serialization/Registry.hpp
    Serialization/Access.hpp
    Serialization/ReadArchive.hpp
    Serialization/WriteArchive.hpp
    Serialization/Core.hpp
)
