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
    include/Sifar/Support/map.hpp
    include/Sifar/Support/deque.hpp
    include/Sifar/Support/stack.hpp
    include/Sifar/Support/queue.hpp

    include/Sifar/Hash.hpp
    include/Sifar/Span.hpp
    include/Sifar/Ref.hpp
    include/Sifar/FactoryTable.hpp
    include/Sifar/DataTrack.hpp
    include/Sifar/Compress.hpp
    include/Sifar/Common.hpp
    include/Sifar/Utility.hpp
    include/Sifar/ApplyFunctor.hpp
    include/Sifar/RegistryBase.hpp
    include/Sifar/NativeSaveLoad.hpp
    include/Sifar/Registry.hpp
    include/Sifar/TypeRegistry.hpp
    include/Sifar/ArchiveTrait.hpp
    include/Sifar/Strict.hpp
    include/Sifar/PolymorphicArchive.hpp
    include/Sifar/Access.hpp
    include/Sifar/Serializable.hpp
    include/Sifar/Polymorphic.hpp
    include/Sifar/ArchiveBase.hpp
    include/Sifar/ReadArchive.hpp
    include/Sifar/WriteArchive.hpp
    include/Sifar/UnifiedData.hpp
    include/Sifar/SerializatonBase.hpp
    include/Sifar/Memory.hpp
    include/Sifar/DataTrackBase.hpp
    include/Sifar/PolymorphicTrait.hpp
	
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
