# declare version
cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# glad gl loader with all ARB and EXT functions.
project(glad)

# use those files for the project
set(HEADERS
	glad/glad.h
	KHR/khrplatform.h
)
set(SOURCES
	glad.c
)

# prepend directories to the files
list(TRANSFORM HEADERS PREPEND include/)
list(TRANSFORM SOURCES PREPEND src/)

# create the static library
add_library(glad STATIC ${SOURCES} ${HEADERS})
target_include_directories(glad PUBLIC include)
target_compile_features(glad PUBLIC cxx_std_17)

if(NOT GLAD_CUSTOM_OUTPUT)
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/bin/debug/")
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/bin/release/")
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}/build/bin/MinSizeRel/")
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}/build/bin/RelWithDebInfo/")
else()
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${GLAD_CUSTOM_OUTPUT_DEBUG}")
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${GLAD_CUSTOM_OUTPUT_RELEASE}")
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${GLAD_CUSTOM_OUTPUT_MINSIZEREL}")
	set_target_properties(glad PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GLAD_CUSTOM_OUTPUT_RELWITHDEBINFO}")
endif()
