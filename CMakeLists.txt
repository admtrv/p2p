# CMAkeLists.txt

cmake_minimum_required(VERSION 3.22)
project(pks)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "*.cpp")

list(FILTER SOURCES EXCLUDE REGEX "main\\.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*cmake-build-debug.*")

list(APPEND SOURCES main.cpp)
message(STATUS "Sources: ${SOURCES}")

add_executable(p2p ${SOURCES})
