cmake_minimum_required(VERSION 3.8)
project(ClientServer)

set(CMAKE_CXX_STANDARD 11)

set(Boost_INCLUDE_DIR /Users/cshardware/boost_1_81_0)
set(Boost_LIBRARY_DIR /Users/cshardware/boost_1_81_0/arm64)
find_package(Boost COMPONENTS filesystem system regex REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIR} )

add_executable(client Client.cpp)

add_executable(server JFTPS.cpp)

target_link_libraries( server ${Boost_LIBRARIES} )
target_link_libraries( client ${Boost_LIBRARIES} )
