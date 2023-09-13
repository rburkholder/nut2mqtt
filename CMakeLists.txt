# nut2mqtt (solution root)
cmake_minimum_required (VERSION 3.13)

PROJECT(nut2mqtt)

add_definitions(-pthread)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")

add_subdirectory(src)

