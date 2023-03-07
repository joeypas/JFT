cmake_minimum_required(VERSION 3.8)
project(ClientServer)

set(CMAKE_CXX_STANDARD 11)

set(Boost_INCLUDE_DIR /Users/cshardware/boost_1_81_0)
set(Boost_LIBRARY_DIR /Users/cshardware/boost_1_81_0/arm64)
find_package(Boost COMPONENTS filesystem system regex REQUIRED)
include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIR} )

set(SOURCE_FILES Server.cpp Client.cpp)

add_library(jft STATIC ${SOURCE_FILES})

target_link_libraries( jft ${Boost_LIBRARIES} )

add_executable(ServerTest ServerTest.cpp)
add_executable(ClientTest ClientTest.cpp)

target_link_libraries(ServerTest jft)
target_link_libraries(ClientTest jft)

