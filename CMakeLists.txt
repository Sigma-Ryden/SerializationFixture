cmake_minimum_required(VERSION 3.5)

project(SerializationFixture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SF_HEADERS include/SF/*.hpp)
file(GLOB_RECURSE AUTOMATION_SOURCES include/Automation/*.cpp)
file(GLOB_RECURSE SF_TESTING_SOURCES test/*.cpp)

add_executable(${PROJECT_NAME} ${SF_HEADERS} ${AUTOMATION_SOURCES} ${SF_TESTING_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include PRIVATE test)
