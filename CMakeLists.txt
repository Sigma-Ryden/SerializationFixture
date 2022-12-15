cmake_minimum_required(VERSION 3.5)

project(Serialization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SIFAR
    include/Sifar/Detail/Meta.hpp
    include/Sifar/Detail/MetaMacro.hpp
    include/Sifar/Detail/UndefAllMacro.hpp

	include/Sifar/Support/vector.hpp
	include/Sifar/Support/array.hpp
	include/Sifar/Support/string.hpp
	include/Sifar/Support/pair.hpp
	include/Sifar/Support/tuple.hpp
	include/Sifar/Support/list.hpp
	include/Sifar/Support/forward_list.hpp
	include/Sifar/Support/set.hpp
	include/Sifar/Support/unique_ptr.hpp
	include/Sifar/Support/shared_ptr.hpp
	include/Sifar/Support/weak_ptr.hpp
	include/Sifar/Support/map.hpp
	include/Sifar/Support/deque.hpp
	include/Sifar/Support/stack.hpp
	include/Sifar/Support/queue.hpp

	include/Sifar/Dynamic/FactoryTable.hpp
	include/Sifar/Dynamic/Registry.hpp
	include/Sifar/Dynamic/RegistryBase.hpp
	include/Sifar/Dynamic/PolymorphicArchive.hpp
	include/Sifar/Dynamic/PolymorphicTrait.hpp
	include/Sifar/Dynamic/Polymorphic.hpp

	include/Sifar/Memory/Span.hpp
	include/Sifar/Memory/Ref.hpp
	include/Sifar/Memory/Memory.hpp

	include/Sifar/Hash.hpp
    include/Sifar/DataTrack.hpp
    include/Sifar/Compress.hpp
    include/Sifar/Common.hpp
    include/Sifar/Utility.hpp
    include/Sifar/ApplyFunctor.hpp
    include/Sifar/NativeSaveLoad.hpp
	include/Sifar/TypeRegistry.hpp
    include/Sifar/ArchiveTrait.hpp
    include/Sifar/Strict.hpp
    include/Sifar/Access.hpp
    include/Sifar/Serializable.hpp
    include/Sifar/ArchiveBase.hpp
    include/Sifar/ReadArchive.hpp
    include/Sifar/WriteArchive.hpp
    include/Sifar/UnifiedData.hpp
    include/Sifar/SerializatonBase.hpp
    include/Sifar/DataTrackBase.hpp
	include/Sifar/Config.hpp

    include/Sifar/Core.hpp
)

add_executable(${PROJECT_NAME}
	test/polymorphic.cpp
	# test/virtual_base.cpp
	# test/demo.cpp
	# test/simple.cpp
	# test/tracking.cpp
	# test/inheritance.cpp
	# test/multi_array.cpp
	# test/tracking_virtual.cpp
	# test/ref_tracking.cpp
	# test/tracking_shared_ptr.cpp
	${SIFAR}
)
