# [[Root]]
cmake_minimum_required(VERSION 3.5)

project(Eightser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")


# [[Options]]
option(EIGHTSER_BUILD_SHARED_LIBS "Build shared libraies by Default" ON)
option(EIGHTSER_EXTENDED_SERIALIZATION "Build extended serialization by Default" OFF)
option(EIGHTSER_BUILD_TEST_LIBS "Build testing libraies by Default" OFF)


# [[Defaults]]
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Binaries")
    if(APPLE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Binaries/${EIGHTSER_TESTS_LAUNCHER_NAME}.app/Contents/Frameworks")
    else()
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Binaries")
    endif()
endif()

if(LINUX)
    set(EIGHTSER_RPATH "$ORIGIN")
elseif(APPLE)
    set(EIGHTSER_RPATH "@loader_path/../Frameworks")
endif()


# [[Binaries]]
if(EIGHTSER_BUILD_SHARED_LIBS)
    set(PROJECT_LIBS_TYPE SHARED)
else()
    set(PROJECT_LIBS_TYPE STATIC)
endif()


file(GLOB_RECURSE PROJECT_SOURCES_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")
add_library(Eightser ${PROJECT_LIBS_TYPE} ${PROJECT_SOURCES_FILES})
target_include_directories(Eightser PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

if(EIGHTSER_BUILD_SHARED_LIBS)
    set_target_properties(Eightser PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS $<BOOL:${WIN32}>)
endif()

if(EIGHTSER_EXTENDED_SERIALIZATION)
    target_compile_definitions(Eightser PUBLIC "EIGHTSER_EXTENDED_SERIALIZATION")
endif()


# [[Tests]]
if(EIGHTSER_BUILD_TEST_LIBS)
    file(GLOB_RECURSE PROJECT_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/test/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/test/*.hpp")
    add_library(EightserTests ${PROJECT_LIBS_TYPE} ${PROJECT_TEST_SOURCES})

    if(PROJECT_IS_TOP_LEVEL)
        # you should manually download Eightest if not
        set(EIGHTEST_RUN_MODULE "EightserTests")
        add_subdirectory("Eightest")
    endif()

    target_link_libraries(EightserTests PUBLIC Eightser Eightest)
    target_include_directories(EightserTests PRIVATE "${CMAKE_CURRENT_LIST_DIR}/test")
    set_target_properties(EightserTests PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "${EIGHTSER_RPATH}")

    # we should define this macro to enable additional checks for testing
    target_compile_definitions(Eightser PUBLIC "EIGHTSER_DEBUG")
endif()
