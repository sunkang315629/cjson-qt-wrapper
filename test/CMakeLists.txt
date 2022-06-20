cmake_minimum_required(VERSION 3.10)

project(test-project)

file(GLOB SRCS *.cpp)
file(GLOB HEADERS *.h)

link_libraries(c-json json-wrapper gtest gtest_main)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS} ${HEADERS})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
