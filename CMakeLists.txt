cmake_minimum_required(VERSION 3.5)

project(Serialization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SIRAF
    include/Siraf/Detail/Meta.hpp
    include/Siraf/Detail/MetaMacro.hpp
    include/Siraf/Detail/MacroUndef.hpp

    include/Siraf/Core/ArchiveBase.hpp
    include/Siraf/Core/ArchiveTrait.hpp
    include/Siraf/Core/Config.hpp
    include/Siraf/Core/PolymorphicTrait.hpp
    include/Siraf/Core/PolymorphicArchive.hpp
    include/Siraf/Core/TypeRegistry.hpp
    include/Siraf/Core/Serialization.hpp
    include/Siraf/Core/SerializatonBase.hpp
    include/Siraf/Core/Memory.hpp
    include/Siraf/Core/TypeCore.hpp
    include/Siraf/Core/Hash.hpp

    include/Siraf/Dynamic/Instantiable.hpp
    include/Siraf/Dynamic/InstantiableRegistry.hpp
    include/Siraf/Dynamic/ExternRegistry.hpp
    include/Siraf/Dynamic/AnyRegistry.hpp

    include/Siraf/Utility/Utility.hpp
    include/Siraf/Utility/Span.hpp
    include/Siraf/Utility/Ref.hpp

    include/Siraf/DataTrack.hpp
    include/Siraf/Compress.hpp
    include/Siraf/Common.hpp
    include/Siraf/ApplyFunctor.hpp
    include/Siraf/NativeSaveLoad.hpp
    include/Siraf/Strict.hpp
    include/Siraf/ReadArchive.hpp
    include/Siraf/WriteArchive.hpp
    include/Siraf/ExternSerialization.hpp
    include/Siraf/StreamWrapper.hpp
    include/Siraf/DataTrackBase.hpp
    include/Siraf/Hierarchy.hpp
    include/Siraf/Serializable.hpp
    include/Siraf/HierarchyTrack.hpp

    include/Siraf/Core.hpp
)

set(SIRAF_SUPPORT
    include/Siraf/Support/Detail/Meta.hpp
    include/Siraf/Support/vector.hpp
    include/Siraf/Support/array.hpp
    include/Siraf/Support/string.hpp
    include/Siraf/Support/pair.hpp
    include/Siraf/Support/tuple.hpp
    include/Siraf/Support/list.hpp
    include/Siraf/Support/forward_list.hpp
    include/Siraf/Support/set.hpp
    include/Siraf/Support/unique_ptr.hpp
    include/Siraf/Support/shared_ptr.hpp
    include/Siraf/Support/weak_ptr.hpp
    include/Siraf/Support/map.hpp
    include/Siraf/Support/deque.hpp
    include/Siraf/Support/stack.hpp
    include/Siraf/Support/queue.hpp
    include/Siraf/Support/valarray.hpp
    include/Siraf/Support/bitset.hpp
    include/Siraf/Support/atomic.hpp
    include/Siraf/Support/complex.hpp
    include/Siraf/Support/optional.hpp
    include/Siraf/Support/variant.hpp
    include/Siraf/Support/any.hpp
)

set(AUTO
    include/Auto/TestingCore.hpp
    include/Auto/TestingCore.cpp
)

add_executable(${PROJECT_NAME}
    # test/demo/demo.cpp
    # test/demo/ref_tracking.cpp
    # test/demo/example.cpp

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
    test/TestLibrary.cpp

    test/SirafTestingBase.hpp
    test/main.cpp

    ${SIRAF}
    ${SIRAF_SUPPORT}
    ${AUTO}
)
