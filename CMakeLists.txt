# base
cmake_minimum_required(VERSION 3.5)

project(SerializationFixtureTesting LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# ~base

# packages
set(Automation_DIR ${CMAKE_CURRENT_LIST_DIR}/../Automation/cmake)
find_package(Automation REQUIRED)

set(SerializationFixture_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(SerializationFixture REQUIRED)
# ~packages

# test
include_directories(test)

file(GLOB_RECURSE ${PROJECT_NAME}_TEST_SOURCES test/*.cpp)
file(GLOB_RECURSE ${PROJECT_NAME}_TEST_HEADERS test/*.hpp) # optionaly

# we should define this macro to enable additional checks for testing
add_compile_definitions(SF_DEBUG)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_TEST_SOURCES}
    ${${PROJECT_NAME}_TEST_HEADERS} # optionaly
    ${SerializationFixture_HEADER_FILES} # optionaly
)
target_link_libraries(${PROJECT_NAME} Automation SerializationFixture)
# ~test
