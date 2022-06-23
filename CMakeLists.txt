cmake_minimum_required(VERSION 3.22)

project(cstar_compiler)
set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "src/**/*.cpp" "src/*.cpp")
include_directories(include)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
