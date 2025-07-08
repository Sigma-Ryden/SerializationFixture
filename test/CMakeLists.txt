# [[Root]]
project(SFTesting)


# [[Subdirectories]]
add_subdirectory("Automation")


# [[Binaries]]
file(GLOB_RECURSE PROJECT_TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_executable("${PROJECT_NAME}" ${PROJECT_TEST_SOURCES})


# [[Dependencies]]
target_link_libraries("${PROJECT_NAME}" PRIVATE Automation SF)
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_LIST_DIR}")


# [[Definitions]]
# we should define this macro to enable additional checks for testing
target_compile_definitions("${PROJECT_NAME}" PUBLIC
    SF_DEBUG
    # SF_EXTENDED_SERIALIZATION
)
