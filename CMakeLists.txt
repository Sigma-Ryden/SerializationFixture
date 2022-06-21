cmake_minimum_required(VERSION 3.5)

project(Serializer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(${PROJECT_NAME} test/polymorphic.cpp
    include/Serialization/Detail/Tools.hpp
    include/Serialization/Detail/Pointer.hpp
    include/Serialization/Detail/Meta.hpp
    include/Serialization/Detail/MacroScope.hpp
    include/Serialization/Detail/MacroUnscope.hpp
    include/Serialization/Support/vector.hpp
    include/Serialization/Support/array.hpp
    include/Serialization/Support/string.hpp
    include/Serialization/UndefMacro.hpp
    include/Serialization/Hash.hpp
    include/Serialization/Span.hpp
    include/Serialization/Ref.hpp
    include/Serialization/Registry.hpp
    include/Serialization/Access.hpp
    include/Serialization/ReadArchive.hpp
    include/Serialization/WriteArchive.hpp
    include/Serialization/Core.hpp
)
