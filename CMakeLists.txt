cmake_minimum_required(VERSION 3.18)

project(Minecraft)

file(GLOB_RECURSE MINECRAFT_FILES
    src/*.cpp
    src/*.h
    res/*.*
    deps/PerlinNoise/PerlinNoise.h
    deps/stb_image/stb_image.cpp
    deps/stb_image/stb_image.h)

foreach(item IN ITEMS ${MINECRAFT_FILES})
    get_filename_component(src_path ${item} PATH)
    string(REPLACE ${CMAKE_SOURCE_DIR} "" group_path ${src_path})
    string(REPLACE / \\ group_path ${group_path})
    source_group(${group_path} FILES ${item})
endforeach()

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    file(COPY res/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/res/)
endif()

add_executable(Minecraft ${MINECRAFT_FILES})

add_subdirectory(deps/GLFW/)
add_subdirectory(deps/Glad/)

target_link_libraries(Minecraft PRIVATE glfw glad)
target_include_directories(Minecraft PUBLIC
    deps/GLFW/
    deps/Glad/
    deps/glm/
    deps/PerlinNoise/
    deps/stb_image/)

target_precompile_headers(Minecraft PRIVATE src/pch.h)
target_compile_options(Minecraft PRIVATE -W4 -MP -std:c++17)

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Minecraft)
endif()
