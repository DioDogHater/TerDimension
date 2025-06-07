cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 11)

set(CMAKE_BUILD_TYPE "Debug")

project(TerDimension)

add_executable(main src/main.c)
target_link_libraries(main m)
#target_link_libraries(main pthread)