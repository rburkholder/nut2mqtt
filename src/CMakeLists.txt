# nut2mqtt/src
cmake_minimum_required (VERSION 3.13)

PROJECT(nut2mqtt)

# https://cmake.org/cmake/help/v3.6/module/FindBoost.html
#set(Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
#set(Boost_DEBUG 1)
#set(Boost_REALPATH ON)
#set(BOOST_ROOT "/usr/local")
#set(Boost_DETAILED_FAILURE_MSG ON)
set(BOOST_INCLUDEDIR "/usr/local/include/boost")
set(BOOST_LIBRARYDIR "/usr/local/lib")

#find_package(Boost 1.81.0 REQUIRED COMPONENTS program_options thread filesystem log)
find_package(Boost REQUIRED COMPONENTS program_options thread filesystem log)

set(
  file_hpp
    Config.hpp
    Loop.hpp
  )

set(
  file_cpp
    Config.cpp
    Loop.cpp
    main.cpp
  )

add_executable(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

#target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_LOG_DYN_LINK )

target_link_directories(
  ${PROJECT_NAME} PUBLIC
    /usr/local/lib
  )

target_link_libraries(
  ${PROJECT_NAME}
    libnutclient.a
    libou_mqtt.a
    libpaho-mqtt3c.a
    ${Boost_LIBRARIES}
  )

