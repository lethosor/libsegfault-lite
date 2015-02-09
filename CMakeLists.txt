cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(libsegfault-lite)
set(CMAKE_BUILD_TYPE Release)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

include_directories(include)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -fPIC")
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.4")
    endif()
endif()

set(LIBRARY_SOURCES src/main.cpp)
if(UNIX)
    list(APPEND LIBRARY_SOURCES src/signal-unix.cpp)
else()
    message(FATAL_ERROR Platform not supported)
endif()

add_library(segfault-lite SHARED ${LIBRARY_SOURCES})

# Ugly hack to work around GCC not supporting -arch properly
if(APPLE)
    add_library(segfault-lite32 SHARED ${LIBRARY_SOURCES})
    add_dependencies(segfault-lite32 segfault-lite)
    set_target_properties(segfault-lite32 PROPERTIES
        COMPILE_FLAGS "-m32"
        LINK_FLAGS "-m32"
    )
    add_custom_command(
        TARGET segfault-lite32
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename libsegfault-lite.dylib libsegfault-lite64.dylib
    )
    add_custom_command(
        TARGET segfault-lite32
        POST_BUILD
        COMMAND lipo -create libsegfault-lite64.dylib libsegfault-lite32.dylib -o libsegfault-lite.dylib
        COMMENT "Creating universal library"
    )
endif()

option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES)
    add_executable(good-example examples/good.cpp examples/boilerplate.cpp)
    add_executable(bad-example examples/bad.cpp examples/boilerplate.cpp)
    add_executable(c-example examples/c-example.c)
    add_dependencies(good-example segfault-lite)
    add_dependencies(bad-example segfault-lite)
    add_dependencies(c-example segfault-lite)

    add_executable(info examples/info.cpp)
    target_link_libraries(info segfault-lite)
    if(APPLE)
        add_dependencies(info segfault-lite32)
    endif()
endif()
