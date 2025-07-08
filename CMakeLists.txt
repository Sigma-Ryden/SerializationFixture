# [[Root]]
cmake_minimum_required(VERSION 3.5)

project(SF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# [[Binaries]]
file(GLOB_RECURSE PROJECT_SOURCES_FILES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp")
add_library("${PROJECT_NAME}" ${PROJECT_SOURCES_FILES})
target_include_directories("${PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")


# [[Tests]]
if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory("test")
endif()
