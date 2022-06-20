cmake_minimum_required(VERSION 3.10)

project(json-object)

link_libraries(c-json)

file(GLOB SRCS *.cpp)
file(GLOB HEADERS *.h)

link_libraries(c-json json-wrapper)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} ${HEADERS})

