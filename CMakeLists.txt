# [[Root]]
cmake_minimum_required(VERSION 3.5)

project(Eightser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()


# [[Options]]
option(EIGHTSER_BUILD_SHARED_LIBS "Build shared libraies by Default" ON)
option(EIGHTSER_EXTENDED_SERIALIZATION "Build extended serialization by Default" OFF)

option(EIGHTSER_BUILD_TEST_LIBS "Build testing libraies by Default" OFF)


# [[Binaries]]
if(EIGHTSER_BUILD_SHARED_LIBS)
    set(PROJECT_LIBS_TYPE SHARED)
else()
    set(PROJECT_LIBS_TYPE STATIC)
endif()


file(GLOB_RECURSE PROJECT_SOURCES_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")
add_library("${PROJECT_NAME}" ${PROJECT_LIBS_TYPE} ${PROJECT_SOURCES_FILES})
target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

if(EIGHTSER_BUILD_SHARED_LIBS)
    set_target_properties("${PROJECT_NAME}" PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(EIGHTSER_EXTENDED_SERIALIZATION)
    target_compile_definitions("${PROJECT_NAME}" PUBLIC "EIGHTSER_EXTENDED_SERIALIZATION")
endif()


# [[Testing]]
if(EIGHTSER_BUILD_TEST_LIBS)
    file(GLOB_RECURSE PROJECT_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/test/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/test/*.hpp")
    add_library("${PROJECT_NAME}Testing" ${PROJECT_LIBS_TYPE} ${PROJECT_TEST_SOURCES})

    if(PROJECT_IS_TOP_LEVEL)
        # you should manually download Eightest if not
        add_subdirectory("Eightest")
    endif()

    target_link_libraries("${PROJECT_NAME}Testing" PUBLIC "${PROJECT_NAME}" "Eightest")
    target_include_directories("${PROJECT_NAME}Testing" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/test")
    set_target_properties("${PROJECT_NAME}Testing" PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "${EIGHTSER_RPATH}")

    # we should define this macro to enable additional checks for testing
    target_compile_definitions("${PROJECT_NAME}" PUBLIC EIGHTSER_DEBUG)
endif()


# [[InplaceTesting]]
if(PROJECT_IS_TOP_LEVEL AND EIGHTSER_BUILD_TEST_LIBS)
    set(PROJECT_JAUNCHER_CPP "${CMAKE_BINARY_DIR}/Launcher.cpp")
    file(WRITE "${PROJECT_JAUNCHER_CPP}"
        "#include <Eightest/Core.hpp>
        #if defined(_WIN32)
            #include <windows.h>
            #define EIGHTSER_LOAD_MODULE(name) LoadLibraryA(#name\".dll\")
        #elif defined(__linux__) || defined(__APPLE__)
            #include <dlfcn.h>
            #define EIGHTSER_LOAD_MODULE(name) dlopen(#name\".so\", RTLD_NOW)
        #endif
        int main() {
            EIGHTSER_LOAD_MODULE(EightserTesting);
            TRY_CATCH(EXECUTE_ALL());
            TESTING_STAT();
            return 0;
        }"
    )

    add_executable("Launcher" "${PROJECT_JAUNCHER_CPP}")

    target_link_libraries("Launcher" PUBLIC "Eightest")    
    if(LINUX)
        target_link_libraries("Launcher" PUBLIC "dl")
    endif()

    if(LINUX)
         set_target_properties("Launcher" PROPERTIES
             BUILD_WITH_INSTALL_RPATH TRUE
             INSTALL_RPATH "$ORIGIN"
         )
    elseif(APPLE)
        set_target_properties("Launcher" PROPERTIES
            MACOSX_BUNDLE TRUE
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "@loader_path/../Frameworks"
        )
    endif()
endif()
