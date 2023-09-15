# nut2mqtt/src
cmake_minimum_required (VERSION 3.13)

#PROJECT(nut2mqtt)

set(
  file_hpp
  )

set( 
  file_cpp
    main.cpp
  )

add_executable(
  ${PROJECT_NAME}
    ${file_h}
    ${file_cpp}
  )

target_link_libraries(
  ${PROJECT_NAME}
    nutclient
    dl
    pthread
  )

