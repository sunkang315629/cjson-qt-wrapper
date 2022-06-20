cmake_minimum_required(VERSION 3.10)

# 为了和Linux系统的cjson库区分，这里取一个不同的名字
project(c-json C)

file(GLOB SRCS *.c)
file(GLOB HEADERS *.h)

add_library(${PROJECT_NAME} STATIC ${SRCS} ${HEADERS})

