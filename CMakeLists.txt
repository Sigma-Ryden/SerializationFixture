cmake_minimum_required(VERSION 3.5)

project(SerializationFixture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SF
    include/SF/Detail/Meta.hpp
    include/SF/Detail/MetaMacro.hpp
    include/SF/Detail/MacroUndef.hpp

    include/SF/Core/ArchiveBase.hpp
    include/SF/Core/ArchiveTrait.hpp
    include/SF/Core/Config.hpp
    include/SF/Core/InstantiableTrait.hpp
    include/SF/Core/PolymorphicArchive.hpp
    include/SF/Core/TypeRegistry.hpp
    include/SF/Core/Serialization.hpp
    include/SF/Core/SerializatonBase.hpp
    include/SF/Core/Memory.hpp
    include/SF/Core/TypeCore.hpp
    include/SF/Core/Hash.hpp

    include/SF/Dynamic/Instantiable.hpp
    include/SF/Dynamic/InstantiableRegistry.hpp
    include/SF/Dynamic/ExternRegistry.hpp
    include/SF/Dynamic/AnyRegistry.hpp

    include/SF/Utility/Utility.hpp
    include/SF/Utility/Span.hpp
    include/SF/Utility/Ref.hpp

    include/SF/DataTrack.hpp
    include/SF/Compress.hpp
    include/SF/Common.hpp
    include/SF/ApplyFunctor.hpp
    include/SF/NativeSaveLoad.hpp
    include/SF/Strict.hpp
    include/SF/ReadArchive.hpp
    include/SF/WriteArchive.hpp
    include/SF/ExternSerialization.hpp
    include/SF/StreamWrapper.hpp
    include/SF/DataTrackBase.hpp
    include/SF/Hierarchy.hpp
    include/SF/Serializable.hpp
    include/SF/HierarchyTrack.hpp

    include/SF/Core.hpp
)

set(SF_SUPPORT
    include/SF/Support/Detail/Meta.hpp
    include/SF/Support/vector.hpp
    include/SF/Support/array.hpp
    include/SF/Support/string.hpp
    include/SF/Support/pair.hpp
    include/SF/Support/tuple.hpp
    include/SF/Support/list.hpp
    include/SF/Support/forward_list.hpp
    include/SF/Support/set.hpp
    include/SF/Support/unique_ptr.hpp
    include/SF/Support/shared_ptr.hpp
    include/SF/Support/weak_ptr.hpp
    include/SF/Support/map.hpp
    include/SF/Support/deque.hpp
    include/SF/Support/stack.hpp
    include/SF/Support/queue.hpp
    include/SF/Support/valarray.hpp
    include/SF/Support/bitset.hpp
    include/SF/Support/atomic.hpp
    include/SF/Support/complex.hpp
    include/SF/Support/optional.hpp
    include/SF/Support/variant.hpp
    include/SF/Support/any.hpp
)

set(AUTO
    include/Auto/TestingCore.hpp
    include/Auto/TestingCore.cpp
)

include_directories(test)

add_executable(${PROJECT_NAME}
    # test/demo/example.cpp
    # test/demo/demo.cpp

    test/TestNumber.cpp
    test/TestEnum.cpp
    test/TestUserType.cpp
    test/TestVector.cpp
    test/TestArray.cpp
    test/TestValarray.cpp
    test/TestString.cpp
    test/TestDeque.cpp
    test/TestQueue.cpp
    test/TestPriorityQueue.cpp
    test/TestStack.cpp
    test/TestBitset.cpp
    test/TestList.cpp
    test/TestForwardList.cpp
    test/TestMap.cpp
    test/TestSet.cpp
    test/TestVariant.cpp
    test/TestOptional.cpp
    test/TestPair.cpp
    test/TestTuple.cpp
    test/TestMemory.cpp
    test/TestAtomic.cpp
    test/TestComplex.cpp
    test/TestAny.cpp
    test/TestDynamicArray.cpp
    test/TestPolymorphic.cpp
    test/TestTracking.cpp

    test/TestLibrary/BaseObject.hpp
    test/TestLibrary/BaseObject.cpp
    test/TestLibrary/DerivedObject.hpp
    test/TestLibrary/DerivedObject.cpp
    test/TestLibrary/TestLibrary.cpp

    test/SFTestingBase.hpp
    test/main.cpp

    ${SF}
    ${SF_SUPPORT}
    ${AUTO}
)
