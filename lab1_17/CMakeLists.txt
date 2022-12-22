cmake_minimum_required(VERSION 3.23)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-static")

add_executable(main main.cpp employee.h)
add_executable(creator creator.cpp employee.h)
add_executable(reporter reporter.cpp employee.h)