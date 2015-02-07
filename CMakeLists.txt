cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(libsegfault-lite)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

include_directories(include)

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -fPIC")
endif()

set(LIBRARY_SOURCES src/main.cpp)
if(UNIX)
    list(APPEND LIBRARY_SOURCES src/signal-unix.cpp)
else()
    message(FATAL_ERROR Platform not supported)
endif()

add_library(segfault-lite SHARED ${LIBRARY_SOURCES})

option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES)
    add_executable(good-example examples/good.cpp)
    add_executable(bad-example examples/bad.cpp)
endif()
