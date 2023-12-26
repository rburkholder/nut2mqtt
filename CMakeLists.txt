# nut2mqtt (solution root)
cmake_minimum_required (VERSION 3.13)

PROJECT(base)

add_definitions(-pthread)

#set(CMAKE_CXX_STANDARD 20)
# beaglebone when loaded with AM335x 11.7 has gcc-10 which supports 14
# beaglebone when loaded with AM335x 12.2 has gcc-12 which supports 17
# https://www.beagleboard.org/distros/am335x-12-2-2023-10-07-4gb-emmc-iot-flasher
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")

add_subdirectory(src)
