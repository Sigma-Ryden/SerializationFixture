cmake_minimum_required(VERSION 3.5)

project(Serialization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
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
	# test/polymorphic.cpp
    # test/virtual_base.cpp
	# test/demo.cpp
	# test/simple.cpp
    # test/tracking.cpp
	# test/inheritance.cpp
    # test/multi_array.cpp
    # test/tracking_virtual.cpp
    # test/ref_tracking.cpp
    # test/tracking_shared_ptr.cpp
	# test/example.cpp
	test/testing.cpp
	${SIRAF}
)
