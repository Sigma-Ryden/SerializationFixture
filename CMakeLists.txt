cmake_minimum_required(VERSION 3.5)

project(Serialization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SIFAR
    include/Sifar/Detail/Counter.hpp
    include/Sifar/Detail/Pointer.hpp
    include/Sifar/Detail/Meta.hpp
    include/Sifar/Detail/Invoker.hpp
    include/Sifar/Detail/MacroScope.hpp
    include/Sifar/Detail/MacroUnscope.hpp

    include/Sifar/Support/vector.hpp
    include/Sifar/Support/array.hpp
    include/Sifar/Support/string.hpp
    include/Sifar/Support/pair.hpp
    include/Sifar/Support/tuple.hpp
    include/Sifar/Support/list.hpp
    include/Sifar/Support/forward_list.hpp
    include/Sifar/Support/set.hpp
    include/Sifar/Support/unique_ptr.hpp
    include/Sifar/Support/map.hpp
    include/Sifar/Support/deque.hpp
    include/Sifar/Support/stack.hpp
    include/Sifar/Support/queue.hpp

    include/Sifar/UndefMacro.hpp
    include/Sifar/Hash.hpp
    include/Sifar/Span.hpp
    include/Sifar/Ref.hpp
    include/Sifar/Utility.hpp
    include/Sifar/RegistryBase.hpp
    include/Sifar/Registry.hpp
    include/Sifar/TypeRegistry.hpp
    include/Sifar/Access.hpp
    include/Sifar/ReadArchive.hpp
    include/Sifar/WriteArchive.hpp

    include/Sifar/Core.hpp
)

add_executable(${PROJECT_NAME}
   # test/polymorphic.cpp
   # test/class_export.cpp
   # test/virtual_base.cpp
   # test/demo.cpp
   # test/simple.cpp
   # test/tracking.cpp
   # test/inheritance.cpp
   # test/multi_array.cpp
    test/tracking_virtual.cpp
   # test/class_auto_indexing.cpp
    ${SIFAR}
)
