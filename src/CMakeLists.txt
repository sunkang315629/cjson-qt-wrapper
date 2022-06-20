cmake_minimum_required(VERSION 3.10)

project(json-wrapper CXX)

file(GLOB SRCS *.cpp)
file(GLOB HEADERS *.h)

add_library(${PROJECT_NAME} STATIC ${SRCS} ${HEADERS})
target_link_libraries(${PROJECT_NAME} c-json)
