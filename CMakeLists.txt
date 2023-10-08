# nut2mqtt (solution root)
cmake_minimum_required (VERSION 3.13)

PROJECT(base)

add_definitions(-pthread)

#set(CMAKE_CXX_STANDARD 20)
# beaglebone has gcc-10 which supports 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")

add_subdirectory(src)
