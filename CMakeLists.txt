cmake_minimum_required(VERSION 2.8)

project(rle)

set (CMAKE_CXX_STANDARD 11)

set(SOURCES
    src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
