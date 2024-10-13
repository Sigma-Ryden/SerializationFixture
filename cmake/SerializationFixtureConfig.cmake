set(XXPACKAGE_NAME SerializationFixture)


include(FeatureSummary)
set_package_properties(${XXPACKAGE_NAME} PROPERTIES)


set(${XXPACKAGE_NAME}_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include")
set(${XXPACKAGE_NAME}_SOURCE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../src") # optionaly


include_directories(${${XXPACKAGE_NAME}_INCLUDE_DIRS}) # optionaly


file(GLOB_RECURSE ${XXPACKAGE_NAME}_HEADER_FILES ${${XXPACKAGE_NAME}_INCLUDE_DIRS}/*.hpp)
file(GLOB_RECURSE ${XXPACKAGE_NAME}_SOURCE_FILES ${${XXPACKAGE_NAME}_SOURCE_DIRS}/*.cpp) # optionaly


add_library(${XXPACKAGE_NAME} SHARED ${${XXPACKAGE_NAME}_SOURCE_FILES}) # optionaly


unset(XXPACKAGE_NAME)
