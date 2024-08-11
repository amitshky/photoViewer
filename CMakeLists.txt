cmake_minimum_required(VERSION 3.28)

project(photoViewer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
set(CMAKE_BUILD_TYPE Debug)

if(GNU OR Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS};-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow;")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG};-g;-O0;")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE};-O3;")
endif()

add_subdirectory(src)
add_subdirectory(ext)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    "src/"
    "./ext/raylib/src/"
)

target_link_libraries(
    ${PROJECT_NAME}
    raylib
)
