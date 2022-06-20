cmake_minimum_required(VERSION 3.10)

project(cjson-qt-wrapper C CXX)

# Linux 系统相关设置
if (CMAKE_HOST_UNIX)
set(CXX_FLAGS
 -g
 -Wall
 -Wextra
 -Werror
 #-Wconversion
 -Wno-unused-parameter
# -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
# -Wshadow
 -Wwrite-strings
 -march=native
 -std=c++11
 -rdynamic
)

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
endif()

# windows环境相关设置
if (CMAKE_HOST_WIN32)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
    set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/3rdparty)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

enable_testing()
add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(test)
