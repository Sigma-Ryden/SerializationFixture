cmake_minimum_required(VERSION 3.5)

project(SerializationFixture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SF
    include/SF/Detail/Meta.hpp
    include/SF/Detail/MetaMacro.hpp
    include/SF/Detail/MacroUndef.hpp
    include/SF/Detail/Preprocessor.hpp

    include/SF/Core/ArchiveBase.hpp
    include/SF/Core/ArchiveTraits.hpp
    include/SF/Core/Config.hpp
    include/SF/Core/InstantiableTraits.hpp
    include/SF/Core/PolymorphicArchive.hpp
    include/SF/Core/Serialization.hpp
    include/SF/Core/SerializatonBase.hpp
    include/SF/Core/Memory.hpp
    include/SF/Core/TypeCore.hpp
    include/SF/Core/Hash.hpp

    include/SF/Dynamic/Instantiable.hpp
    include/SF/Dynamic/InstantiableRegistry.hpp
    include/SF/Dynamic/ExternRegistry.hpp
    include/SF/Dynamic/AnyRegistry.hpp

    include/SF/Utility/Span.hpp
    include/SF/Utility/Functional.hpp
    include/SF/Utility/BitPack.hpp
    include/SF/Utility/Alias.hpp
    include/SF/Utility/Detail/Macro.hpp

    include/SF/Binary.hpp
    include/SF/DataTrack.hpp
    include/SF/Compress.hpp
    include/SF/Common.hpp
    include/SF/ApplyFunctor.hpp
    include/SF/NativeSaveLoad.hpp
    include/SF/Strict.hpp
    include/SF/IArchive.hpp
    include/SF/OArchive.hpp
    include/SF/StreamWrapper.hpp
    include/SF/DataTrackBase.hpp
    include/SF/Hierarchy.hpp
    include/SF/Serializable.hpp
    include/SF/HierarchyTrack.hpp
    include/SF/Union.hpp
    include/SF/Aggregate.hpp

    include/SF/Core.hpp
)

set(SF_BUILT_IN
    include/SF/BuiltIn/Detail/Meta.hpp
    include/SF/BuiltIn/vector.hpp
    include/SF/BuiltIn/array.hpp
    include/SF/BuiltIn/string.hpp
    include/SF/BuiltIn/pair.hpp
    include/SF/BuiltIn/tuple.hpp
    include/SF/BuiltIn/list.hpp
    include/SF/BuiltIn/forward_list.hpp
    include/SF/BuiltIn/set.hpp
    include/SF/BuiltIn/unique_ptr.hpp
    include/SF/BuiltIn/shared_ptr.hpp
    include/SF/BuiltIn/weak_ptr.hpp
    include/SF/BuiltIn/map.hpp
    include/SF/BuiltIn/deque.hpp
    include/SF/BuiltIn/stack.hpp
    include/SF/BuiltIn/queue.hpp
    include/SF/BuiltIn/priority_queue.hpp
    include/SF/BuiltIn/valarray.hpp
    include/SF/BuiltIn/bitset.hpp
    include/SF/BuiltIn/atomic.hpp
    include/SF/BuiltIn/complex.hpp
    include/SF/BuiltIn/optional.hpp
    include/SF/BuiltIn/variant.hpp
    include/SF/BuiltIn/any.hpp
)

set(AUTO
    include/AutoTesting/Core.hpp
    include/AutoTesting/Core.cpp
)

include_directories(test)

add_executable(${PROJECT_NAME}
    test/TestNumber.cpp
    test/TestEnum.cpp
    test/TestUserType.cpp
    test/TestUnion.cpp
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
    test/TestAggregate.cpp
    test/TestAlias.cpp
    test/TestBitPack.cpp

    test/TestLibrary/BaseObject.hpp
    test/TestLibrary/BaseObject.cpp
    test/TestLibrary/DerivedObject.hpp
    test/TestLibrary/DerivedObject.cpp
    test/TestLibrary.cpp

    test/TestExample.cpp

    test/SFTestingBase.hpp
    test/main.cpp

    ${SF}
    ${SF_BUILT_IN}
    ${AUTO}
)
