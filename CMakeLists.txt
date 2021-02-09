cmake_minimum_required(VERSION 3.13...3.18 FATAL_ERROR)

project(Minecraft)

file(GLOB "MINECRAFT_FILES" "src/*.cpp"
                            "src/*.h"
                            "res/images/*.*"
                            "res/shaders/*.*"
                            "deps/PerlinNoise/PerlinNoise.h"
                            "deps/stb_image/stb_image.cpp"
                            "deps/stb_image/stb_image.h")

foreach(item IN ITEMS ${MINECRAFT_FILES})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE "/" "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach()

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    file(COPY "res/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res/")
endif()

add_executable(Minecraft ${MINECRAFT_FILES})

add_subdirectory("deps/GLFW/")
add_subdirectory("deps/Glad/")

target_link_libraries(Minecraft PRIVATE glfw glad)
target_include_directories(Minecraft PUBLIC "deps/GLFW/"
                                    "deps/Glad/"
                                    "deps/glm/"
                                    "deps/PerlinNoise/"
                                    "deps/stb_image/")

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Minecraft)

    set_target_properties(Minecraft PROPERTIES COMPILE_FLAGS "/Yupch.h")
    set_source_files_properties("src/pch.cpp" PROPERTIES COMPILE_FLAGS "/Ycpch.h")

    target_compile_options(Minecraft PRIVATE "/W4" "/MP" "/std:c++17")
endif()