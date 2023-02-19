cmake_minimum_required(VERSION 3.5)

project(Serialization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SIRAF
    include/Siraf/Detail/Meta.hpp
    include/Siraf/Detail/MetaMacro.hpp
    include/Siraf/Detail/UndefAllMacro.hpp

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
	include/Siraf/Support/bitset.hpp

    include/Siraf/Dynamic/FactoryTable.hpp
    include/Siraf/Dynamic/Registry.hpp
    include/Siraf/Dynamic/RegistryBase.hpp
    include/Siraf/Dynamic/PolymorphicArchive.hpp
    include/Siraf/Dynamic/PolymorphicTrait.hpp
    include/Siraf/Dynamic/Polymorphic.hpp

    include/Siraf/Memory/Span.hpp
    include/Siraf/Memory/Ref.hpp
    include/Siraf/Memory/Memory.hpp

    include/Siraf/Hash.hpp
    include/Siraf/DataTrack.hpp
    include/Siraf/Compress.hpp
    include/Siraf/Common.hpp
    include/Siraf/Utility.hpp
    include/Siraf/ApplyFunctor.hpp
    include/Siraf/NativeSaveLoad.hpp
    include/Siraf/TypeRegistry.hpp
    include/Siraf/ArchiveTrait.hpp
    include/Siraf/Strict.hpp
    include/Siraf/Access.hpp
    include/Siraf/Serializable.hpp
    include/Siraf/ArchiveBase.hpp
    include/Siraf/ReadArchive.hpp
    include/Siraf/WriteArchive.hpp
    include/Siraf/ExternSerialization.hpp
    include/Siraf/StreamWrapper.hpp
    include/Siraf/SerializatonBase.hpp
    include/Siraf/DataTrackBase.hpp
    include/Siraf/Config.hpp
    include/Siraf/Detail/StaticMessage.hpp

    include/Siraf/Core.hpp
)

add_executable(${PROJECT_NAME}
	# test/demo/polymorphic.cpp
	# test/demo/virtual_base.cpp
	# test/demo/demo.cpp
	# test/demo/simple.cpp
	# test/demo/tracking.cpp
	# test/demo/inheritance.cpp
	# test/demo/multi_array.cpp
	# test/demo/tracking_virtual.cpp
	# test/demo/ref_tracking.cpp
	# test/demo/tracking_shared_ptr.cpp
	# test/demo/example.cpp

	test/TestModule.h
	test/TestModule.cpp

	test/TestNumber.h
	test/TestNumber.cpp
	test/TestEnum.h
	test/TestEnum.cpp
	test/TestUserType.h
	test/TestUserType.cpp

	test/TestVector.h
	test/TestVector.cpp
	test/TestArray.h
	test/TestArray.cpp

	test/TestString.h
	test/TestString.cpp

	test/TestDeque.h
	test/TestDeque.cpp

	test/TestQueue.h
	test/TestQueue.cpp
	test/TestStack.h
	test/TestStack.cpp

	test/TestBitset.h
	test/TestBitset.cpp

	test/TestList.h
	test/TestList.cpp
	test/TestForwardList.h
	test/TestForwardList.cpp

	test/TestMap.h
	test/TestMap.cpp
	test/TestSet.h
	test/TestSet.cpp

	test/TestPair.h
	test/TestPair.cpp
	test/TestTuple.h
	test/TestTuple.cpp

	test/TestMemory.h
	test/TestMemory.cpp

	test/main.cpp

	${SIRAF}
)
