cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_STANDARD 11)

# To enable Debug symbols in the executable, uncomment this line
#set(CMAKE_BUILD_TYPE "Debug")

# To enable profiling
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")

# To maximise performance at the cost of compilation speed
#set(CMAKE_BUILD_TYPE "Release")

project(TerDimension)

# /build/main
add_executable(main src/main.c)

# Linux maths
target_link_libraries(main m)

# Uncomment this line if you have SDL installed system-wide
#target_link_libraries(main SDL)
