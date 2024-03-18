cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(project_name miniserver)
project(${project_name})

# set(CMAKE_C_COMPILER mingw-w64-x86_64-gcc)

set(SOURCES src/app.c)
set(HEADERS src/app.h)

add_executable(${project_name} ${SOURCES} ${HEADERS})
